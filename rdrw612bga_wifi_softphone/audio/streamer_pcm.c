/*
 * Copyright 2022-2023, 2025-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "app_definitions.h"
#include "board.h"
#include "streamer_pcm.h"
#include "fsl_debug_console.h"
#include "fsl_codec_common.h"
#include "main.h"

#define RX_SLOT_COUNT  (3U)
#define RX_BUFFER_SIZE (AUDIO_FRAME_SIZE * RX_SLOT_COUNT)

typedef enum
{
    PCM_SUCCESS = 0,
    PCM_ERROR   = -1,
} pcm_status_t;

static pcm_rtos_t pcmHandle = {0};

extern codec_handle_t codecHandle;

/* RX ring buffer indices -- byte offsets into s_RxBuffer[RX_BUFFER_SIZE] */
static volatile uint32_t s_RxWriteIndex = 0U;    /* ISR writer  */
static volatile uint32_t s_RxReadIndex  = 0U;    /* task writer */
static volatile bool s_RxOverrun        = false; /* internal: self-clearing after recovery */
static volatile bool s_RxOverrunFlag    = false; /* external: sticky, cleared by get_and_clear */

SDK_ALIGN(static uint8_t s_RxBuffer[RX_BUFFER_SIZE], 16);

/* TX DMA buffers: one slot per SDK queue entry */
SDK_ALIGN(static uint8_t s_TxBuffer[I2S_NUM_BUFFERS][AUDIO_FRAME_SIZE], 16);
static uint32_t s_TxWriteIndex    = 0U; /* next slot to fill */
static volatile bool s_TxUnderrun = false;

static void TxCallback(I2S_Type *base, i2s_dma_handle_t *handle, status_t completionStatus, void *userData)
{
    pcm_rtos_t *pcm = (pcm_rtos_t *)userData;
    BaseType_t reschedule;

    xSemaphoreGiveFromISR(pcm->semaphoreTX, &reschedule);
    portYIELD_FROM_ISR(reschedule);
}

static void RxCallback(I2S_Type *base, i2s_dma_handle_t *handle, status_t completionStatus, void *userData)
{
    pcm_rtos_t *pcm       = (pcm_rtos_t *)userData;
    BaseType_t reschedule = pdFALSE;

    /* Advance write index */
    uint32_t next = s_RxWriteIndex + AUDIO_FRAME_SIZE;
    if (next >= RX_BUFFER_SIZE)
        next = 0U;

    /* Overrun: write is about to lap the read index */
    if (next == s_RxReadIndex)
    {
        s_RxOverrun     = true;
        s_RxOverrunFlag = true;
    }

    s_RxWriteIndex = next;

    /* Queue next DMA transfer */
    i2s_transfer_t rxTransfer;
    rxTransfer.data     = &s_RxBuffer[next];
    rxTransfer.dataSize = AUDIO_FRAME_SIZE;
    I2S_RxTransferReceiveDMA(base, handle, rxTransfer);

    xSemaphoreGiveFromISR(pcm->semaphoreRX, &reschedule);
    portYIELD_FROM_ISR(reschedule);
}

void streamer_pcm_init(void)
{
    DMA_Init(DEMO_DMA);

    /* TX path */
    DMA_EnableChannel(DEMO_DMA, DEMO_I2S_TX_CHANNEL);
    DMA_SetChannelPriority(DEMO_DMA, DEMO_I2S_TX_CHANNEL, kDMA_ChannelPriority3);
    DMA_CreateHandle(&pcmHandle.i2sTxDmaHandle, DEMO_DMA, DEMO_I2S_TX_CHANNEL);

    /* RX path */
    DMA_EnableChannel(DEMO_DMA, DEMO_I2S_RX_CHANNEL);
    DMA_SetChannelPriority(DEMO_DMA, DEMO_I2S_RX_CHANNEL, kDMA_ChannelPriority2);
    DMA_CreateHandle(&pcmHandle.i2sRxDmaHandle, DEMO_DMA, DEMO_I2S_RX_CHANNEL);

    NVIC_SetPriority(DMA0_IRQn, 2U);

    pcmHandle.isFirstTx   = 1;
    pcmHandle.isFirstRx   = 1;
    pcmHandle.semaphoreTX = NULL;
    pcmHandle.semaphoreRX = NULL;
    s_RxReadIndex         = 0U;
    s_RxWriteIndex        = 0U;
}

int streamer_pcm_tx_open(uint32_t num_buffers)
{
    /* Counting semaphore: one count per free DMA buffer slot. Starts at I2S_NUM_BUFFERS */
    pcmHandle.semaphoreTX = xSemaphoreCreateCounting(I2S_NUM_BUFFERS, I2S_NUM_BUFFERS);
    return 0;
}

int streamer_pcm_rx_open(uint32_t num_buffers)
{
    /* Counting semaphore: one count per filled slot. Starts at 0 */
    pcmHandle.semaphoreRX = xSemaphoreCreateCounting(RX_SLOT_COUNT, 0U);
    return 0;
}

void streamer_pcm_tx_close(void)
{
    /* Stop playback */
    if (pcmHandle.i2sTxHandle.state != 0)
    {
        I2S_TransferAbortDMA(DEMO_I2S_TX, &pcmHandle.i2sTxHandle);
    }
    vSemaphoreDelete(pcmHandle.semaphoreTX);

    pcmHandle.isFirstTx = 1;
    s_TxWriteIndex      = 0U;
    s_TxUnderrun        = false;
}

void streamer_pcm_rx_close(void)
{
    pcmHandle.isFirstRx = 1;

    /* Stop I2S RX capture */
    if (pcmHandle.i2sRxHandle.state != 0)
    {
        I2S_TransferAbortDMA(DEMO_I2S_RX, &pcmHandle.i2sRxHandle);
    }
    vSemaphoreDelete(pcmHandle.semaphoreRX);

    s_RxReadIndex   = 0U;
    s_RxWriteIndex  = 0U;
    s_RxOverrun     = false;
    s_RxOverrunFlag = false;
}

bool streamer_pcm_rx_overrun_get_and_clear(void)
{
    bool ov         = s_RxOverrunFlag;
    s_RxOverrunFlag = false;
    return ov;
}

int streamer_pcm_write(uint8_t *data, uint32_t size)
{
    assert(size == AUDIO_FRAME_SIZE);
    assert((size % 32) == 0);

    uint8_t *dst = s_TxBuffer[s_TxWriteIndex];
    i2s_transfer_t txTransfer;

    txTransfer.data     = dst;
    txTransfer.dataSize = size;

    if (xSemaphoreTake(pcmHandle.semaphoreTX, pdMS_TO_TICKS(100)) != pdTRUE)
        return PCM_ERROR;

    bool wasIdle = (pcmHandle.i2sTxHandle.state == 0U);

    memcpy(dst, data, size);

    /* Submit to the SDK DMA queue. With the counting semaphore gate a free slot
     * is always available here, so kStatus_I2S_Busy cannot occur. */
    status_t status = I2S_TxTransferSendDMA(DEMO_I2S_TX, &pcmHandle.i2sTxHandle, txTransfer);
    if (status != kStatus_Success)
    {
        /* Submit failed unexpectedly -- return the slot count we consumed. */
        xSemaphoreGive(pcmHandle.semaphoreTX);
        return PCM_ERROR;
    }

    if (!pcmHandle.isFirstTx && wasIdle)
        s_TxUnderrun = true;
    pcmHandle.isFirstTx = 0;

    s_TxWriteIndex = (s_TxWriteIndex + 1U) % I2S_NUM_BUFFERS;
    return PCM_SUCCESS;
}

bool streamer_pcm_tx_underrun_get_and_clear(void)
{
    bool un      = s_TxUnderrun;
    s_TxUnderrun = false;
    return un;
}

int streamer_pcm_read(uint8_t *data, uint32_t size)
{
    assert(size == AUDIO_FRAME_SIZE);

    if (pcmHandle.isFirstRx)
    {
        pcmHandle.isFirstRx = 0;

        i2s_transfer_t rxTransfer;

        /* Pre-queue slots 0 and 1 so DMA capture starts immediately.
         * RxCallback will advance s_RxWriteIndex as each slot completes. */
        rxTransfer.data     = &s_RxBuffer[0];
        rxTransfer.dataSize = AUDIO_FRAME_SIZE;
        I2S_RxTransferReceiveDMA(DEMO_I2S_RX, &pcmHandle.i2sRxHandle, rxTransfer);

        rxTransfer.data     = &s_RxBuffer[AUDIO_FRAME_SIZE];
        rxTransfer.dataSize = AUDIO_FRAME_SIZE;
        I2S_RxTransferReceiveDMA(DEMO_I2S_RX, &pcmHandle.i2sRxHandle, rxTransfer);

        /* Advance write index to slot 1; first callback will advance it to slot 2 */
        s_RxWriteIndex = AUDIO_FRAME_SIZE;

        /* Fall through and block for first real frame */
    }

    /* Wait for a completed DMA slot */
    if (xSemaphoreTake(pcmHandle.semaphoreRX, pdMS_TO_TICKS(100)) != pdTRUE)
        return PCM_ERROR;

    /* Drop-oldest recovery */
    if (s_RxOverrun)
    {
        /* Jump to the slot just before the write index (the freshest captured data). */
        uint32_t w = s_RxWriteIndex;
        if (w == 0U)
            s_RxReadIndex = RX_BUFFER_SIZE - AUDIO_FRAME_SIZE;
        else
            s_RxReadIndex = w - AUDIO_FRAME_SIZE;
        s_RxOverrun = false; /* consumed -- clear internal flag */
    }

    memcpy(data, &s_RxBuffer[s_RxReadIndex], AUDIO_FRAME_SIZE);

    /* Advance read index */
    uint32_t next = s_RxReadIndex + AUDIO_FRAME_SIZE;
    if (next >= RX_BUFFER_SIZE)
        next = 0U;
    s_RxReadIndex = next;

    return PCM_SUCCESS;
}

int streamer_pcm_tx_setparams(uint32_t sample_rate, uint32_t bit_width, uint8_t num_channels, int volume)
{
    int ret = 0;

    pcmHandle.sample_rate  = sample_rate;
    pcmHandle.bit_width    = bit_width;
    pcmHandle.num_channels = num_channels;

    /* Configure TX (I2S master, plays back audio).
     * masterSlave = kI2S_MasterSlaveNormalMaster; mode = kI2S_ModeI2sClassic;
     * dataLength = 16; frameLength = 32; fifoLevel = 4. */
    I2S_TxGetDefaultConfig(&pcmHandle.tx_config);
    pcmHandle.tx_config.divider     = DEMO_I2S_CLOCK_DIVIDER;
    pcmHandle.tx_config.masterSlave = DEMO_I2S_TX_MODE;
    if (pcmHandle.num_channels == 1)
    {
        pcmHandle.tx_config.oneChannel = true;
    }
    I2S_TxInit(DEMO_I2S_TX, &pcmHandle.tx_config);
    I2S_TxTransferCreateHandleDMA(DEMO_I2S_TX, &pcmHandle.i2sTxHandle, &pcmHandle.i2sTxDmaHandle, TxCallback,
                                  (void *)&pcmHandle);

    ret = CODEC_SetMute(&codecHandle, ALL_CHANNELS, true);
    ret = streamer_pcm_set_volume(volume);
    if (ret != kStatus_Success)
    {
        return 1;
    }

    return 0;
}

int streamer_pcm_rx_setparams(uint32_t sample_rate, uint32_t bit_width, uint8_t num_channels)
{
    pcmHandle.sample_rate  = sample_rate;
    pcmHandle.bit_width    = bit_width;
    pcmHandle.num_channels = num_channels;

    /* Configure RX (I2S slave, captures mic audio).
     * masterSlave = kI2S_MasterSlaveNormalSlave; mode = kI2S_ModeI2sClassic;
     * dataLength = 16; frameLength = 32; fifoLevel = 4. */
    I2S_RxGetDefaultConfig(&pcmHandle.rx_config);
    pcmHandle.rx_config.divider     = DEMO_I2S_CLOCK_DIVIDER;
    pcmHandle.rx_config.masterSlave = DEMO_I2S_RX_MODE;
    if (pcmHandle.num_channels == 1)
    {
        pcmHandle.rx_config.oneChannel = true;
    }
    I2S_RxInit(DEMO_I2S_RX, &pcmHandle.rx_config);
    I2S_RxTransferCreateHandleDMA(DEMO_I2S_RX, &pcmHandle.i2sRxHandle, &pcmHandle.i2sRxDmaHandle, RxCallback,
                                  (void *)&pcmHandle);

    return 0;
}

void streamer_pcm_getparams(uint32_t *sample_rate, uint32_t *bit_width, uint8_t *num_channels)
{
    *sample_rate  = pcmHandle.sample_rate;
    *bit_width    = pcmHandle.bit_width;
    *num_channels = pcmHandle.num_channels;
}

int streamer_pcm_mute(bool mute)
{
    status_t ret;
    ret = CODEC_SetMute(&codecHandle, ALL_CHANNELS, mute);
    if (ret != kStatus_Success)
    {
        return 1;
    }

    return 0;
}

int streamer_pcm_set_volume(int volume)
{
    int channel;
    channel =
        (pcmHandle.num_channels == 1) ? kCODEC_PlayChannelHeadphoneLeft | kCODEC_PlayChannelSpeakerLeft : ALL_CHANNELS;

    if (volume <= 0)
        CODEC_SetMute(&codecHandle, ALL_CHANNELS, true);
    else
        CODEC_SetVolume(&codecHandle, channel, volume > CODEC_VOLUME_MAX_VALUE ? CODEC_VOLUME_MAX_VALUE : volume);

    return 0;
}
