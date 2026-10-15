/**
 * @file opus.c Opus Audio Codec
 *
 * Copyright (C) 2010 Alfred E. Heggestad
 */

#include <re.h>
#include <baresip.h>
#include <opus.h>
#include "opus.h"


/**
 * @defgroup opus opus
 *
 * The OPUS audio codec
 *
 * Supported version: libopus 1.0.0 or later
 *
 * Configuration options:
 *
 \verbatim
  opus_stereo        yes     # Request peer to send stereo
  opus_sprop_stereo  yes     # Sending stereo
  opus_bitrate    128000     # Average bitrate in [bps]
  opus_cbr        {yes,no}   # Constant Bitrate (inverse of VBR)
  opus_inbandfec  {yes,no}   # Enable inband Forward Error Correction (FEC)
  opus_dtx        {yes,no}   # Enable Discontinuous Transmission (DTX)
  opus_complexity {0-10}     # Encoder's computational complexity (10 max)
  opus_application {audio, voip} # Encoder's intended application
  opus_packet_loss {0-100}   # Expected packet loss for FEC
 \endverbatim
 *
 * References:
 *
 *    RFC 6716  Definition of the Opus Audio Codec
 *    RFC 7587  RTP Payload Format for the Opus Speech and Audio Codec
 *
 *    http://opus-codec.org/downloads/
 */


static bool opus_mirror;
static char fmtp[256];
static char fmtp_mirror[256];

uint32_t opus_complexity = 0;
opus_int32 opus_application = OPUS_APPLICATION_VOIP;
opus_int32 opus_packet_loss = 0;


static int opus_fmtp_enc(struct mbuf *mb, const struct sdp_format *fmt,
			 bool offer, void *arg)
{
	bool mirror;

	(void)arg;
	(void)offer;

	if (!mb || !fmt)
		return 0;

	mirror = !offer && str_isset(fmtp_mirror);

	return mbuf_printf(mb, "a=fmtp:%s %s\r\n",
			   fmt->id, mirror ? fmtp_mirror : fmtp);
}


static struct aucodec opus = {
	.name      = "opus",
	.srate     = 48000,
	.crate     = 48000,
	.ch        = 2,
	.pch       = 2,
	.fmtp      = fmtp,
	.encupdh   = opus_encode_update,
	.ench      = opus_encode_frm,
	.decupdh   = opus_decode_update,
	.dech      = opus_decode_frm,
	.plch      = opus_decode_pkloss,
};


void opus_mirror_params(const char *x)
{
	if (!opus_mirror)
		return;

	info("opus: mirror parameters: \"%s\"\n", x);

	str_ncpy(fmtp_mirror, x, sizeof(fmtp_mirror));
}


static int module_init(void)
{
	// struct conf *conf = conf_cur();
	// uint32_t value;
	char *p = fmtp;
	bool stereo = false, sprop_stereo = false, cbr = false, fec = true, dtx = true;
	uint32_t max_bitrate = 24000;
	uint32_t playback_rate = 16000;
	uint32_t capture_rate = 16000;
	uint32_t samplerate = 16000;
	// struct pl pl;
	int n = 0;

	fmtp[0] = '\0';
	fmtp_mirror[0] = '\0';

//	conf_get_bool(conf, "opus_stereo", &stereo);
//	conf_get_bool(conf, "opus_sprop_stereo", &sprop_stereo);

	if (!stereo || !sprop_stereo)
		opus.ch = 1;

	/* always set stereo parameter first */
	n = re_snprintf(p, sizeof(fmtp) - str_len(p),
			"stereo=%d;sprop-stereo=%d", stereo, sprop_stereo);
	if (n <= 0)
		return ENOMEM;

	p += n;

	if (max_bitrate) {

		n = re_snprintf(p, sizeof(fmtp) - str_len(p),
				";maxaveragebitrate=%d", max_bitrate);
		if (n <= 0)
			return ENOMEM;

		p += n;
	}
	if (samplerate) {

		if ((samplerate != 8000) && (samplerate != 12000) && (samplerate != 16000) &&
		    (samplerate != 24000) && (samplerate != 48000)) {
			warning("opus: invalid samplerate: %d\n", samplerate);
			return EINVAL;
		}
		opus.srate = samplerate;
	}

	if (cbr) {

		n = re_snprintf(p, sizeof(fmtp) - str_len(p),
				";cbr=%d", cbr);
		if (n <= 0)
			return ENOMEM;

		p += n;
	}

	if (fec) {

		n = re_snprintf(p, sizeof(fmtp) - str_len(p),
				";useinbandfec=%d", fec);
		if (n <= 0)
			return ENOMEM;

		p += n;
	}

	if (dtx) {

		n = re_snprintf(p, sizeof(fmtp) - str_len(p),
				";usedtx=%d", dtx);
		if (n <= 0)
			return ENOMEM;

		p += n;
	}

	if (playback_rate) {

		n = re_snprintf(p, sizeof(fmtp) - str_len(p),
				";maxplaybackrate=%d", playback_rate);
		if (n <= 0)
			return ENOMEM;

		p += n;
	}

	if (capture_rate) {

		n = re_snprintf(p, sizeof(fmtp) - str_len(p),
				";sprop-maxcapturerate=%d", capture_rate);
		if (n <= 0)
			return ENOMEM;
	}

//	(void)conf_get_bool(conf, "opus_mirror", &opus_mirror);

	if (opus_mirror) {
		opus.fmtp = NULL;
		opus.fmtp_ench = opus_fmtp_enc;
	}

//	(void)conf_get_u32(conf, "opus_complexity", &opus_complexity);

	if (opus_complexity > 10)
		opus_complexity = 10;

//	if (!conf_get(conf, "opus_application", &pl)) {
//		if (!pl_strcasecmp(&pl, "audio"))
//			opus_application = OPUS_APPLICATION_AUDIO;
//		else if (!pl_strcasecmp(&pl, "voip"))
//			opus_application = OPUS_APPLICATION_VOIP;
//		else {
//			warning("opus: unknown encoder application: %r\n",
//					&pl);
//			return EINVAL;
//		}
//	}
//
//	if (0 == conf_get_u32(conf, "opus_packet_loss", &value)) {
//
//		if (value > 100)
//			opus_packet_loss = 100;
//		else
//			opus_packet_loss = value;
//	}

	debug("opus: fmtp=\"%s\"\n", fmtp);

	aucodec_register(baresip_aucodecl(), &opus);

	return 0;
}


static int module_close(void)
{
	aucodec_unregister(&opus);

	return 0;
}


EXPORT_SYM const struct mod_export DECL_EXPORTS(opus) = {
	"opus",
	"audio codec",
	module_init,
	module_close,
};
