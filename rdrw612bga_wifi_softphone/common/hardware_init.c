/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*${header:start}*/
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_power.h"
#include "fsl_codec_common.h"
#include "fsl_codec_adapter.h"
#include "fsl_wm8904.h"
#include "app.h"
#include "fsl_i2s_bridge.h"
/*${header:end}*/

#include "fsl_inputmux.h"
#include "fsl_dma.h"
#include "fsl_gpio.h"
#include "display_support.h"

#define WM8904_MICBIAS_CTRL_0 (0x06)

/*${variable:start}*/
codec_handle_t codecHandle = {0};

wm8904_config_t wm8904Config = {
    .i2cConfig          = {.codecI2CInstance = BOARD_CODEC_I2C_INSTANCE},
    .recordSource       = kWM8904_RecordSourceLineInput,
    .recordChannelLeft  = kWM8904_RecordChannelLeft1,
    .recordChannelRight = kWM8904_RecordChannelRight1,
    .playSource         = kWM8904_PlaySourceDAC,
    .slaveAddress       = WM8904_I2C_ADDRESS,
    .protocol           = kWM8904_ProtocolI2S,
#if (DEMO_AUDIO_SAMPLE_RATE == 48000)
    .format             = {.sampleRate = kWM8904_SampleRate48kHz, .bitWidth = kWM8904_BitWidth16},
#elif (DEMO_AUDIO_SAMPLE_RATE == 24000)
    .format             = {.sampleRate = kWM8904_SampleRate24kHz, .bitWidth = kWM8904_BitWidth16},
#elif (DEMO_AUDIO_SAMPLE_RATE == 16000)
    .format             = {.sampleRate = kWM8904_SampleRate16kHz, .bitWidth = kWM8904_BitWidth16},
#elif (DEMO_AUDIO_SAMPLE_RATE == 12000)
    .format             = {.sampleRate = kWM8904_SampleRate12kHz, .bitWidth = kWM8904_BitWidth16},
#elif (DEMO_AUDIO_SAMPLE_RATE == 8000)
    .format             = {.sampleRate = kWM8904_SampleRate8kHz, .bitWidth = kWM8904_BitWidth16},
#else
#error "Unsupported DEMO_AUDIO_SAMPLE_RATE value"
#endif
    .master             = false,
};
codec_config_t boardCodecConfig = {.codecDevType = kCODEC_WM8904, .codecDevConfig = &wm8904Config};
/*${variable:end}*/

/*${function:start}*/
static void i2c_release_bus_delay(void)
{
    uint32_t i = 0;
    for (i = 0; i < 100; i++)
    {
        __NOP();
    }
}

void BOARD_I2C_ReleaseBus(void)
{
    uint8_t i = 0;

    GPIO_PortInit(GPIO, BOARD_CODEC_I2C_SDA_PORT);
    GPIO_PortInit(GPIO, BOARD_CODEC_I2C_SCL_PORT);

    BOARD_InitI2CPinsAsGPIO();

    /* Drive SDA low first to simulate a start */
    GPIO_PinWrite(GPIO, BOARD_CODEC_I2C_SDA_PORT, BOARD_CODEC_I2C_SDA_PIN, 0U);
    i2c_release_bus_delay();

    /* Send 9 pulses on SCL */
    for (i = 0; i < 9; i++)
    {
        GPIO_PinWrite(GPIO, BOARD_CODEC_I2C_SCL_PORT, BOARD_CODEC_I2C_SCL_PIN, 0U);
        i2c_release_bus_delay();

        GPIO_PinWrite(GPIO, BOARD_CODEC_I2C_SDA_PORT, BOARD_CODEC_I2C_SDA_PIN, 1U);
        i2c_release_bus_delay();

        GPIO_PinWrite(GPIO, BOARD_CODEC_I2C_SCL_PORT, BOARD_CODEC_I2C_SCL_PIN, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    /* Send stop */
    GPIO_PinWrite(GPIO, BOARD_CODEC_I2C_SCL_PORT, BOARD_CODEC_I2C_SCL_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(GPIO, BOARD_CODEC_I2C_SDA_PORT, BOARD_CODEC_I2C_SDA_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(GPIO, BOARD_CODEC_I2C_SCL_PORT, BOARD_CODEC_I2C_SCL_PIN, 1U);
    i2c_release_bus_delay();

    GPIO_PinWrite(GPIO, BOARD_CODEC_I2C_SDA_PORT, BOARD_CODEC_I2C_SDA_PIN, 1U);
    i2c_release_bus_delay();
}

int BOARD_CODEC_Init(void)
{
    if (CODEC_Init(&codecHandle, &boardCodecConfig) != kStatus_Success)
    {
        return -1;
    }

    /* Invert the DAC data in order to output signal with correct polarity - set DACL_DATINV and DACR_DATINV = 1 */
    if (WM8904_WriteRegister((wm8904_handle_t *)codecHandle.codecDevHandle, WM8904_AUDIO_IF_0, 0x1850) !=
        kStatus_WM8904_Success)
    {
        return -1;
    }

    /* Enable MICBIAS output voltage for Headset mic. MICBIAS_ENA = 1 */
    if (WM8904_WriteRegister((wm8904_handle_t *)codecHandle.codecDevHandle, WM8904_MICBIAS_CTRL_0, 0x1) !=
        kStatus_WM8904_Success)
    {
        return -1;
    }

    /* Boost left input PGA gain to +22.3 dB (LIN_VOL = 0x1D) for headset electret mic. */
    if (WM8904_WriteRegister((wm8904_handle_t *)codecHandle.codecDevHandle, WM8904_ANALOG_LEFT_IN_0, 0x001DU) !=
        kStatus_WM8904_Success)
    {
        return -1;
    }

    /* Initial volume kept low for hearing safety. */
    if (CODEC_SetVolume(&codecHandle, ALL_CHANNELS, DEMO_VOLUME) != kStatus_Success)
    {
        return -1;
    }

    return 0;
}

void BOARD_InitHardware(void)
{
    BOARD_InitBootPins();
    if (BOARD_IS_XIP())
    {
        BOARD_BootClockRUN();
        CLOCK_EnableClock(kCLOCK_Otp);
        CLOCK_EnableClock(kCLOCK_Els);
        CLOCK_EnableClock(kCLOCK_ElsApb);
        RESET_PeripheralReset(kOTP_RST_SHIFT_RSTn);
        RESET_PeripheralReset(kELS_APB_RST_SHIFT_RSTn);
    }
    else
    {
        BOARD_InitBootClocks();
    }
    BOARD_InitDebugConsole();
    BOARD_I2C_ReleaseBus();
    BOARD_InitI2CPins();

    /* attach SFRO clock to I2C2 */
    CLOCK_AttachClk(kSFRO_to_FLEXCOMM2);

    /* attach AUDIO PLL clock to FLEXCOMM1 (I2S1) */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM1);
    /* attach AUDIO PLL clock to FLEXCOMM0 (I2S0) */
    CLOCK_AttachClk(kAUDIO_PLL_to_FLEXCOMM0);

    /* attach AUDIO PLL clock to MCLK */
    CLOCK_AttachClk(kAUDIO_PLL_to_MCLK_CLK);
    CLOCK_SetClkDiv(kCLOCK_DivMclkClk, 1);
    SYSCTL1->MCLKPINDIR = SYSCTL1_MCLKPINDIR_MCLKPINDIR_MASK;

    wm8904Config.i2cConfig.codecI2CSourceClock = BOARD_CODEC_I2C_CLOCK_FREQ;
    wm8904Config.mclk_HZ                       = CLOCK_GetMclkClkFreq();

    /* Set shared signal set 0: SCK, WS from Flexcomm1 */
    I2S_BRIDGE_SetShareSignalSrc(kI2S_BRIDGE_ShareSet0, kI2S_BRIDGE_SignalSCK, kI2S_BRIDGE_Flexcomm1);
    I2S_BRIDGE_SetShareSignalSrc(kI2S_BRIDGE_ShareSet0, kI2S_BRIDGE_SignalWS, kI2S_BRIDGE_Flexcomm1);
    /* Set flexcomm0 SCK, WS from shared signal set 0 */
    I2S_BRIDGE_SetFlexcommSignalShareSet(kI2S_BRIDGE_Flexcomm0, kI2S_BRIDGE_SignalSCK, kI2S_BRIDGE_ShareSet0);
    I2S_BRIDGE_SetFlexcommSignalShareSet(kI2S_BRIDGE_Flexcomm0, kI2S_BRIDGE_SignalWS, kI2S_BRIDGE_ShareSet0);

    /* Reset GMDA */
    RESET_PeripheralReset(kGDMA_RST_SHIFT_RSTn);
    /* Keep CAU sleep clock here. */
    /* CPU1 uses Internal clock when in low power mode. */
    POWER_ConfigCauInSleep(false);
    BOARD_InitSleepPinConfig();

    /* Initialize the audio codec (WM8904) so that codecHandle is valid
     * before any streamer or PCM calls attempt to use it. */
    BOARD_CODEC_Init();

#if (DEMO_PANEL == DEMO_PANEL_LCD_PAR_S035)
    BOARD_InitLcd1Pins();
    CLOCK_AttachClk(kTDDR_MCI_FLEXSPI_to_LCD_CLK);
    /* Clock source is 320mHz, divided by 4 which gives LCDIC 80mHz source. */
    CLOCK_SetClkDiv(kCLOCK_DivLcdClk, 4);
    RESET_PeripheralReset(kLCDIC_RST_SHIFT_RSTn);
#else
    BOARD_InitLcd0Pins();
    CLOCK_AttachClk(kMAIN_CLK_to_LCD_CLK);
    CLOCK_SetClkDiv(kCLOCK_DivLcdClk, 13);
    RESET_PeripheralReset(kLCDIC_RST_SHIFT_RSTn);
#endif

    /* Use 16 MHz clock for the FLEXCOMM2 */
    CLOCK_AttachClk(kSFRO_to_FLEXCOMM2);

    /* DMA */
    DMA_Init(DMA1);
    /* GPIO. */
    GPIO_PortInit(GPIO, 0);
    GPIO_PortInit(GPIO, 1);

    /* inputmux */
    INPUTMUX_Init(INPUTMUX);
    RESET_PeripheralReset(kINPUTMUX_RST_SHIFT_RSTn);
}
/*${function:end}*/
