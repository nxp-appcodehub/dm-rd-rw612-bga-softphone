/*
 * Copyright 2022, 2025-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _APP_DEFINITIONS_H_
#define _APP_DEFINITIONS_H_

/*${header:start}*/
#include "fsl_gpio.h"
/*${header:end}*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/

#define DEMO_I2S_MASTER_CLOCK_FREQUENCY CLOCK_GetMclkClkFreq()
#define DEMO_I2S_TX                     (I2S1)
#define DEMO_I2S_RX                     (I2S0)
#define DEMO_I2S_CLOCK_DIVIDER          (DEMO_I2S_MASTER_CLOCK_FREQUENCY / DEMO_AUDIO_SAMPLE_RATE / DEMO_AUDIO_BIT_WIDTH / 2U)
#define DEMO_I2S_TX_CHANNEL             (3)
#define DEMO_I2S_RX_CHANNEL             (0)
#define DEMO_I2S_TX_MODE                kI2S_MasterSlaveNormalMaster
#define DEMO_I2S_RX_MODE                kI2S_MasterSlaveNormalSlave

#define DEMO_DMA                   (DMA0)
#define DEMO_DMIC_RX_CHANNEL       16U
#define DEMO_DMIC_CHANNEL          kDMIC_Channel0
#define DEMO_DMIC_CHANNEL_ENABLE   DMIC_CHANEN_EN_CH0(1)

#define DEMO_AUDIO_BIT_WIDTH   16U
#define DEMO_AUDIO_BYTE_WIDTH  2U
#define DEMO_AUDIO_SAMPLE_RATE 16000U
#define DEMO_MIC_FRAME_SIZE    20U
#define DEMO_MIC_CHANNEL_NUM   1U

#define AUDIO_FRAME_SIZE \
    (DEMO_MIC_CHANNEL_NUM * DEMO_AUDIO_SAMPLE_RATE * DEMO_MIC_FRAME_SIZE / 1000U * DEMO_AUDIO_BYTE_WIDTH)

#define FIFO_DEPTH   (15U)
#define DEMO_VOLUME  (90)
#define ALL_CHANNELS (0xFF)

#define GPIO_PORT 0
#define D3_PIN    15
#define D4_PIN    18
#define D5_PIN    27
#define D6_PIN    6

#define D3_ON()      GPIO_PinWrite(GPIO, GPIO_PORT, D3_PIN, 1)
#define D3_OFF()     GPIO_PinWrite(GPIO, GPIO_PORT, D3_PIN, 0)
#define D3_TOGGLE()  GPIO_PortToggle(GPIO, GPIO_PORT, (1u << D3_PIN))

#define D4_ON()      GPIO_PinWrite(GPIO, GPIO_PORT, D4_PIN, 1)
#define D4_OFF()     GPIO_PinWrite(GPIO, GPIO_PORT, D4_PIN, 0)
#define D4_TOGGLE()  GPIO_PortToggle(GPIO, GPIO_PORT, (1u << D4_PIN))

#define D5_ON()      GPIO_PinWrite(GPIO, GPIO_PORT, D5_PIN, 1)
#define D5_OFF()     GPIO_PinWrite(GPIO, GPIO_PORT, D5_PIN, 0)
#define D5_TOGGLE()  GPIO_PortToggle(GPIO, GPIO_PORT, (1u << D5_PIN))

#define D6_ON()      GPIO_PinWrite(GPIO, GPIO_PORT, D6_PIN, 1)
#define D6_OFF()     GPIO_PinWrite(GPIO, GPIO_PORT, D6_PIN, 0)
#define D6_TOGGLE()  GPIO_PortToggle(GPIO, GPIO_PORT, (1u << D6_PIN))
/*${macro:end}*/

#endif /* _APP_DEFINITIONS_H_ */
