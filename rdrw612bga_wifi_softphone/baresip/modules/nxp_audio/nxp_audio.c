/**
 * @file nxp_audio.c NXP Audio driver for RW612
 *
 * Copyright (C) 2010 Alfred E. Heggestad
 * Copyright 2025-2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <re.h>
#include <rem.h>
#include <baresip.h>
#include "app_streamer.h"
#include "app.h"

struct ausrc_st
{
    const struct ausrc *as; /* Audio source */
    ausrc_read_h *rh;       /* Read handler from baresip */
    void *arg;              /* Argument for read handler */
    uint32_t ptime;         /* Packet time */
    size_t sampc;           /* Samples per packet */
};

struct auplay_st
{
    const struct auplay *ap; /* Audio player */
    auplay_write_h *wh;      /* Write handler from baresip */
    void *arg;               /* Argument for write handler */
    uint32_t ptime;          /* Packet time */
    size_t sampc;            /* Samples per packet */
};

/* Global ausrc/auplay pointers (needed for unregistration) */
static struct ausrc *nxp_ausrc   = NULL;
static struct auplay *nxp_auplay = NULL;

static void ausrc_destructor(void *arg)
{
    (void)arg;

    STREAMER_rx_stop();

    /* Unregister callback */
    streamer_register_baresip_ausrc(NULL, NULL);

    info("nxp_audio: ausrc destroyed\n");
}

static void auplay_destructor(void *arg)
{
    (void)arg;

    STREAMER_tx_stop();

    /* Unregister callback */
    streamer_register_baresip_auplay(NULL, NULL);

    info("nxp_audio: auplay destroyed\n");
}

/**
 * Callback invoked by the streamer when microphone data is available.
 * Called from the RX task context via streamer_network_write().
 */
static void ausrc_callback(const int16_t *samples, uint32_t num_samples, void *arg)
{
    struct ausrc_st *st = arg;
    struct auframe af;

    if (!st || !st->rh)
        return;

    /* Package samples into auframe */
    auframe_init(&af, AUFMT_S16LE, (void *)samples, num_samples, DEMO_AUDIO_SAMPLE_RATE, 1);
    af.timestamp = tmr_jiffies_usec();

    /* Send to baresip audio TX pipeline */
    st->rh(&af, st->arg);
}

/**
 * Callback invoked by the streamer when it needs speaker data.
 * Called from the TX task context via streamer_network_read().
 */
static void auplay_callback(int16_t *samples, uint32_t num_samples, void *arg)
{
    struct auplay_st *st = arg;
    struct auframe af;

    if (!st || !st->wh)
    {
        /* No callback - fill with silence */
        memset(samples, 0, num_samples * sizeof(int16_t));
        return;
    }

    /* Prepare buffer for baresip to fill */
    auframe_init(&af, AUFMT_S16LE, samples, num_samples, DEMO_AUDIO_SAMPLE_RATE, 1);
    af.timestamp = tmr_jiffies_usec();

    /* Get decoded audio from baresip RX pipeline */
    st->wh(&af, st->arg);
}

/**
 * Allocation handler for audio source (mic).
 * Called by baresip when a call needs microphone input.
 */
static int nxp_ausrc_alloc(struct ausrc_st **stp,
                           const struct ausrc *as,
                           struct ausrc_prm *prm,
                           const char *device,
                           ausrc_read_h *rh,
                           ausrc_error_h *errh,
                           void *arg)
{
    struct ausrc_st *st;
    (void)errh;
    (void)device;

    if (!stp || !as || !prm || !rh)
        return EINVAL;

    st = mem_zalloc(sizeof(*st), ausrc_destructor);
    if (!st)
        return ENOMEM;

    st->as    = as;
    st->rh    = rh;
    st->arg   = arg;
    st->ptime = prm->ptime;
    st->sampc = prm->srate * prm->ch * prm->ptime / 1000;

    info("nxp_audio: ausrc started (srate=%u, ch=%u, ptime=%u, sampc=%zu)\n", prm->srate, prm->ch, prm->ptime,
         st->sampc);

    /* Register callback, then start RX I2S path and task */
    streamer_register_baresip_ausrc(ausrc_callback, st);

    if (STREAMER_rx_create() != kStatus_Success)
    {
        warning("nxp_audio: STREAMER_rx_create failed\n");
        mem_deref(st);
        return ENOMEM;
    }

    *stp = st;
    return 0;
}

/**
 * Allocation handler for audio player (speaker).
 * Called by baresip when a call needs speaker output.
 * NOTE: Named nxp_auplay_alloc to avoid conflict with auplay_alloc() in auplay.c
 */
static int nxp_auplay_alloc(struct auplay_st **stp,
                            const struct auplay *ap,
                            struct auplay_prm *prm,
                            const char *device,
                            auplay_write_h *wh,
                            void *arg)
{
    struct auplay_st *st;
    (void)device;

    if (!stp || !ap || !prm || !wh)
        return EINVAL;

    st = mem_zalloc(sizeof(*st), auplay_destructor);
    if (!st)
        return ENOMEM;

    st->ap    = ap;
    st->wh    = wh;
    st->arg   = arg;
    st->ptime = prm->ptime;
    st->sampc = prm->srate * prm->ch * prm->ptime / 1000;

    info("nxp_audio: auplay started (srate=%u, ch=%u, ptime=%u, sampc=%zu)\n", prm->srate, prm->ch, prm->ptime,
         st->sampc);

    /* Register callback, then start TX I2S path and task */
    streamer_register_baresip_auplay(auplay_callback, st);

    if (STREAMER_tx_create() != kStatus_Success)
    {
        warning("nxp_audio: STREAMER_tx_create failed\n");
        mem_deref(st);
        return ENOMEM;
    }

    *stp = st;
    return 0;
}

static int module_init(void)
{
    int err;

    STREAMER_pcm_init();

    /* Register audio source and player drivers with baresip */
    err = ausrc_register(&nxp_ausrc, baresip_ausrcl(), "nxp_mic", nxp_ausrc_alloc);
    err = auplay_register(&nxp_auplay, baresip_auplayl(), "nxp_spk", nxp_auplay_alloc);

    return err;
}

static int module_close(void)
{
    nxp_ausrc  = mem_deref(nxp_ausrc);
    nxp_auplay = mem_deref(nxp_auplay);

    return 0;
}

EXPORT_SYM const struct mod_export DECL_EXPORTS(nxp_audio) = {"nxp_audio", "sound", module_init, module_close};
