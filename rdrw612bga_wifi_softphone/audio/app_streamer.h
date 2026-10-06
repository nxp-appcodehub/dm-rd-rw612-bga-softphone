/*
 * Copyright 2020-2021, 2025-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _APP_STREAMER_H_
#define _APP_STREAMER_H_

#include "fsl_common.h"
#include <stdbool.h>

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/* Baresip callback interface */
typedef void (*baresip_ausrc_callback_h)(const int16_t *samples, uint32_t num_samples, void *arg);
typedef void (*baresip_auplay_callback_h)(int16_t *samples, uint32_t num_samples, void *arg);

/* Register baresip callbacks */
void streamer_register_baresip_ausrc(baresip_ausrc_callback_h callback, void *arg);
void streamer_register_baresip_auplay(baresip_auplay_callback_h callback, void *arg);

/*!
 * @brief Initialize PCM hardware (DMA channels, handles, semaphores).
 *
 * Must be called once at boot before any STREAMER_tx_create / STREAMER_rx_create.
 * Wraps streamer_pcm_init() so callers do not need to include streamer_pcm.h.
 */
void STREAMER_pcm_init(void);

/*!
 * @brief Open I2S TX path, configure codec, and start the TX FreeRTOS task.
 *
 * Called when the auplay driver is allocated (call established, speaker needed).
 *
 * @return kStatus_Success on success.
 */
status_t STREAMER_tx_create(void);

/*!
 * @brief Signal the TX task to stop and wait for it to exit.
 *
 * Called from the auplay destructor before streamer_pcm_tx_close().
 */
void STREAMER_tx_stop(void);

/*!
 * @brief Open I2S RX path, configure codec, and start the RX FreeRTOS task.
 *
 * Called when the ausrc driver is allocated (call established, mic needed).
 *
 * @return kStatus_Success on success.
 */
status_t STREAMER_rx_create(void);

/*!
 * @brief Signal the RX task to stop and wait for it to exit.
 *
 * Called from the ausrc destructor before streamer_pcm_rx_close().
 */
void STREAMER_rx_stop(void);

#if defined(__cplusplus)
}
#endif

#endif /* _APP_STREAMER_H_ */
