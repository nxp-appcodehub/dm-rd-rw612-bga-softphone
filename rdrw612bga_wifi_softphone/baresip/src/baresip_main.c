/**
 * @file src/main.c  Main application code
 *
 * Copyright (C) 2010 - 2021 Alfred E. Heggestad
 * Copyright 2026 NXP
 */

#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_GETOPT
#include <getopt.h>
#endif
#include <re.h>
#include <rem.h>
#include <baresip.h>
#include "core.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app.h"
#include "softphone.h"

#define DEBUG_MODULE ""
#define DEBUG_LEVEL 0
#include <re_dbg.h>

struct netif gnetif; /* network interface structure */
struct ua *app_ua;
uint32_t n_uas;           /**< Number of User Agents              */
struct call *g_active_call  = NULL; /**< Current outgoing/active call   */
struct call *g_pending_call = NULL; /**< Incoming call awaiting answer  */

enum { ASYNC_WORKERS = 4 };

static void call_event_handler(struct call *call, enum call_event ev,
			       const char *str, void *arg);
static void call_dtmf_handler(struct call *call, char key, void *arg);

static void signal_handler(int sig)
{
	static bool term = false;

	if (term) {
		module_app_unload();
		mod_close();
		exit(0);
	}

	term = true;

	info("terminated by signal %d\n", sig);

	ua_stop_all(false);
}


static void ua_exit_handler(void *arg)
{
	(void)arg;
	debug("ua exited -- stopping main runloop\n");

	/* The main run-loop can be stopped now */
	re_cancel();
}


static void tmr_quit_handler(void *arg)
{
	(void)arg;

	ua_stop_all(false);
}


#ifdef HAVE_GETOPT
static void usage(void)
{
	(void)re_fprintf(stderr,
			 "Usage: baresip [options]\n"
			 "options:\n"
			 "\t-4               Force IPv4 only\n"
			 "\t-6               Force IPv6 only\n"
			 "\t-a <software>    Specify SIP User-Agent string\n"
			 "\t-d               Daemon\n"
			 "\t-e <commands>    Execute commands (repeat)\n"
			 "\t-f <path>        Config path\n"
			 "\t-m <module>      Pre-load modules (repeat)\n"
			 "\t-p <path>        Audio files\n"
			 "\t-h -?            Help\n"
			 "\t-s               Enable SIP trace\n"
			 "\t-t <sec>         Quit after <sec> seconds\n"
			 "\t-n <net_if>      Specify network interface\n"
			 "\t-u <parameters>  Extra UA parameters\n"
			 "\t-v               Verbose debug\n"
			 "\t-T               Enable timestamps log\n"
			 "\t-c               Disable colored log\n"
			 );
}
#endif

void simple_message_recv_handler(struct ua *ua, const struct pl *peer,
			      const struct pl *ctype,
			      struct mbuf *body, void *arg)
{
	char from_buf[256];
	pl_strcpy(peer, from_buf, sizeof(from_buf));
	ua_printf(ua, "Incoming message received from %s: %s\n", from_buf, body->buf);
	softphone_notify_message(from_buf, (const char *)body->buf);
}

void baresip_make_call(const char *uri)
{
	int err;

	if (!app_ua) {
		warning("main: baresip_make_call: no UA available\n");
		return;
	}

	if (g_active_call) {
		warning("main: baresip_make_call: call already in progress\n");
		return;
	}

	info("main: initiating outgoing call to: %s\n", uri);

	re_thread_enter();

	err = ua_connect(app_ua, &g_active_call, NULL, uri, VIDMODE_OFF);
	if (err) {
		warning("main: ua_connect failed (%m)\n", err);
		g_active_call = NULL;
		re_thread_leave();
		return;
	}

	call_set_handlers(g_active_call, call_event_handler,
			  call_dtmf_handler, NULL);

	re_thread_leave();

	info("main: outgoing call initiated\n");
}


void baresip_hangup_call(void)
{
	if (!g_active_call) {
		info("main: baresip_hangup_call: no active call\n");
		return;
	}

	info("main: hanging up active call\n");

	re_thread_enter();
	call_hangup(g_active_call, 0, NULL);
	mem_deref(g_active_call);
	g_active_call = NULL;
	re_thread_leave();

	g_active_call = NULL;
}


static void call_event_handler(struct call *call, enum call_event ev,
			       const char *str, void *arg)
{
	const char *peer = call_peeruri(call);

	switch (ev) {
	case CALL_EVENT_INCOMING:
		debug("main: CALL_INCOMING: %s\n", str);
		softphone_notify_call_event(SOFTPHONE_CALL_INCOMING, peer, str);
		break;

	case CALL_EVENT_OUTGOING:
		debug("main: CALL_OUTGOING: %s\n", str);
		softphone_notify_call_event(SOFTPHONE_CALL_OUTGOING, peer, str);
		break;

	case CALL_EVENT_RINGING:
		debug("main: CALL_RINGING: %s\n", str);
		softphone_notify_call_event(SOFTPHONE_CALL_RINGING, peer, str);
		break;

	case CALL_EVENT_PROGRESS:
		debug("main: CALL_PROGRESS: %s\n", str);
		softphone_notify_call_event(SOFTPHONE_CALL_PROGRESS, peer, str);
		break;

	case CALL_EVENT_ANSWERED:
		debug("main: CALL_ANSWERED: %s\n", str);
		softphone_notify_call_event(SOFTPHONE_CALL_ANSWERED, peer, str);
		break;

	case CALL_EVENT_ESTABLISHED:
		debug("main: CALL_ESTABLISHED: %s\n", str);
		softphone_notify_call_event(SOFTPHONE_CALL_ESTABLISHED, peer, str);
		break;

	case CALL_EVENT_CLOSED:
		debug("main: CALL_CLOSED: %s\n", str);
		softphone_notify_call_event(SOFTPHONE_CALL_CLOSED, peer, str);
		if (call == g_active_call)
			g_active_call = NULL;
		if (call == g_pending_call)
			g_pending_call = NULL;
		mem_deref(call);
		break;

	case CALL_EVENT_TRANSFER:
		debug("main: CALL_TRANSFER: %s\n", str);
		softphone_notify_call_event(SOFTPHONE_CALL_TRANSFER, peer, str);
		break;

	case CALL_EVENT_TRANSFER_FAILED:
		debug("main: CALL_TRANSFER_FAILED: %s\n", str);
		softphone_notify_call_event(SOFTPHONE_CALL_TRANSFER_FAILED, peer, str);
		break;

	case CALL_EVENT_MENC:
		debug("main: CALL_MENC: %s\n", str);
		softphone_notify_call_event(SOFTPHONE_CALL_MENC, peer, str);
		break;

	default:
		warning("main: unknown call event: %d\n", ev);
		break;

	}
}


static void call_dtmf_handler(struct call *call, char key, void *arg)
{
	(void)arg;

	debug("main: relaying DTMF event: key = '%c'\n", key ? key : '.');

	call_send_digit(call, key);
}


static void event_handler(enum bevent_ev ev, struct bevent *event, void *arg)
{
	int err;
	struct ua   *ua   = bevent_get_ua(event);
	struct call *call = bevent_get_call(event);
	(void)arg;

	switch (ev) {

	case BEVENT_CALL_INCOMING:
		info("main: CALL_INCOMING: peer=%s  -->  local=%s\n",
				call_peeruri(call),
				call_localuri(call));

		call_set_handlers(call, call_event_handler,
				call_dtmf_handler, NULL);

		/* Notify application layer; it takes ownership of answering
		 * if an incoming-call callback is registered. Otherwise
		 * fall back to the original auto-answer behaviour. */
		if (softphone_notify_incoming_call(call_peeruri(call)))
		{
			/* Store call so softphone_answer() can retrieve it */
			g_pending_call = call;
		}
		else
		{
			err = ua_answer(ua, call, VIDMODE_OFF);
			if (err) {
				call_hangup(call, 500, "Server Error");
			}
			else {
				g_active_call = call;
			}
		}
		break;

	default:
		break;
	}
}

void baresip_main(void *arg)
{
	int af = AF_UNSPEC;
	const char *ua_eprm = NULL;
	const char *software =
		"baresip v" BARESIP_VERSION " (" ARCH "/" OS ")";
	const char *execmdv[16];
	const char *net_interface = NULL;
	const char *audio_path = NULL;
	const char *modv[16];
	struct tmr tmr_quit;
	bool sip_trace = false;
	size_t execmdc = 0;
	size_t modc = 0;
	size_t i;
	uint32_t tmo = 0;
	int dbg_level = DBG_INFO;
	enum dbg_flags dbg_flags = DBG_ANSI;

	int err;

	/*
	 * turn off buffering on stdout
	 */
	//setbuf(stdout, NULL);

	(void)re_fprintf(stdout, "baresip v%s"
			 " Copyright (C) 2010 - 2025"
			 " Alfred E. Heggestad et al.\n",
			 baresip_version());


	(void)sys_coredump_set(true);

	err = libre_init();
	if (err)
		goto out;

#ifdef RE_TRACE_ENABLED
	err = re_trace_init("re_trace.json");
	if (err)
		goto out;
#endif

	tmr_init(&tmr_quit);

#ifdef HAVE_GETOPT
	for (;;) {
		const int c = getopt(argc, argv, "46a:de:f:p:hu:n:vst:m:Tc");
		if (0 > c)
			break;

		switch (c) {

		case '?':
		case 'h':
			usage();
			return -2;

		case 'a':
			software = optarg;
			break;

		case '4':
			af = AF_INET;
			break;

		case '6':
			af = AF_INET6;
			break;

		case 'd':
			break;

		case 'e':
			if (execmdc >= RE_ARRAY_SIZE(execmdv)) {
				warning("max %zu commands\n",
					RE_ARRAY_SIZE(execmdv));
				err = EINVAL;
				goto out;
			}
			execmdv[execmdc++] = optarg;
			break;

		case 'f':
			conf_path_set(optarg);
			break;

		case 'm':
			if (modc >= RE_ARRAY_SIZE(modv)) {
				warning("max %zu modules\n",
					RE_ARRAY_SIZE(modv));
				err = EINVAL;
				goto out;
			}
			modv[modc++] = optarg;
			break;

		case 'p':
			audio_path = optarg;
			break;

		case 's':
			sip_trace = true;
			break;

		case 't':
			tmo = atoi(optarg);
			break;

		case 'n':
			net_interface = optarg;
			break;

		case 'u':
			ua_eprm = optarg;
			break;

		case 'v':
			log_enable_debug(true);
			dbg_level = DBG_DEBUG;
			break;

		case 'T':
			log_enable_timestamps(true);
			dbg_flags |= DBG_TIME;
			break;

		case 'c':
			log_enable_color(false);
			dbg_flags &= ~DBG_ANSI;
			break;

		default:
			break;
		}
	}
#endif

	dbg_init(dbg_level, dbg_flags);
//	err = conf_configure();
//	if (err) {
//		warning("main: configure failed: %m\n", err);
//		goto out;
//	}

	re_thread_async_init(ASYNC_WORKERS);

	/*
	 * Set the network interface before initializing the config
	 */
	if (net_interface) {
		struct config *theconf = conf_config();

		str_ncpy(theconf->net.ifname, net_interface,
			 sizeof(theconf->net.ifname));
	}

	/*
	 * Set prefer_ipv6 preferring the one given in -6 argument (if any)
	 */
	if (af != AF_UNSPEC)
		conf_config()->net.af = af;

	/* Configure audio settings for G.711 */
	struct config *cfg = conf_config();

	cfg->audio.srate_play	 = DEMO_AUDIO_SAMPLE_RATE;
	cfg->audio.srate_src	 = DEMO_AUDIO_SAMPLE_RATE;
	cfg->audio.channels_play = 1;
	cfg->audio.channels_src	 = 1;
	cfg->audio.src_fmt		 = AUFMT_S16LE;
	cfg->audio.play_fmt		 = AUFMT_S16LE;

	/* Use NXP audio driver */
	str_ncpy(cfg->audio.src_mod, "nxp_mic", sizeof(cfg->audio.src_mod));
	str_ncpy(cfg->audio.src_dev, "default", sizeof(cfg->audio.src_dev));
	str_ncpy(cfg->audio.play_mod, "nxp_spk", sizeof(cfg->audio.play_mod));
	str_ncpy(cfg->audio.play_dev, "default", sizeof(cfg->audio.play_dev));

	/*
	 * Initialise the top-level baresip struct, must be
	 * done AFTER configuration is complete.
	*/
	err = baresip_init(conf_config());
	if (err) {
		warning("main: baresip init failed (%m)\n", err);
		goto out;
	}

	/* Set audio path preferring the one given in -p argument (if any) */
	if (audio_path)
		play_set_path(baresip_player(), audio_path);
	else if (str_isset(conf_config()->audio.audio_path)) {
		play_set_path(baresip_player(),
			      conf_config()->audio.audio_path);
	}

	/* Initialise User Agents */
#ifdef SERVER_MODE
	err = ua_init(software, false, false, true);
#else
	err = ua_init(software, true, false, false);
#endif
	if (err)
		goto out;

	/* NOTE: must be done after all arguments are processed and UA is
		initialized; some modules (eg, ctrl_tcp) can only be preloaded
		when the UA is available */
	if (modc) {

		info("pre-loading modules: %zu\n", modc);

		for (i=0; i<modc; i++) {

			err = module_preload(modv[i]);
			if (err) {
				re_fprintf(stderr,
					   "could not pre-load module"
					   " '%s' (%m)\n", modv[i], err);
			}
		}
	}

	uag_set_exit_handler(ua_exit_handler, NULL);

	if (ua_eprm) {
		err = uag_set_extra_params(ua_eprm);
		if (err)
			goto out;
	}

	if (sip_trace)
		uag_enable_sip_trace(true);

	/* Load modules */
	err = conf_modules();
	if (err)
		goto out;

	info("baresip is ready.\n");

	info("Main: Register Text Message Listener\n");
	err = message_listen(baresip_message(), simple_message_recv_handler, NULL);
	if (err != 0) {
		warning("Failed to register handler for MESSAGE RX (err = %d)!\n", err);
	}

    char addr[256];

#ifdef SERVER_MODE
    const char* USERNAME    = "test-secure";
    const char* AUTH_USER   = "test-user";
    const char* AUTH_PASS   = "password-to-test-with-sip-phones";
    const char* SIP_SERVER  = "test-arc.svep.se";
    const uint16_t SIP_PORT = 5061;
	snprintf(addr, sizeof(addr),
		 "<sips:%s@%s:%u;transport=tls>"
		 ";regint=60;ptime=20;answer_any=1"
		 ";auth_user=%s;auth_pass=%s"
		 ";cert=internal;mediaenc=srtp",
		 USERNAME, SIP_SERVER, SIP_PORT,
		 AUTH_USER, AUTH_PASS);
#else
    const char* USERNAME = "0001";
    char local_ip[INET_ADDRSTRLEN] = "0.0.0.0";

    /* Find the first LwIP network interface that is up and has a valid IPv4
     * address. This covers both STA (terminal) mode, where netif_default is
     * set, and AP (access point) mode, where netif_default may be NULL but
     * the UAP netif is registered and active. */
    {
        struct netif *nif;
        NETIF_FOREACH(nif)
        {
            if (netif_is_up(nif) && !ip4_addr_isany(netif_ip4_addr(nif)))
            {
                ip4addr_ntoa_r(netif_ip4_addr(nif), local_ip, sizeof(local_ip));
                break;
            }
        }
        if (local_ip[0] == '0' && local_ip[1] == '.')
        {
            warning("main: no netif with a valid IP found, falling back to 0.0.0.0\n");
        }
    }

    snprintf(addr, sizeof(addr),
             "<sip:%s@%s;transport=udp>"
             ";regint=60;ptime=20;answer_any=1",
             USERNAME, local_ip);
#endif
    info("Main: Register a SIP User-Agent: %s\n", addr);
	/** \note Do not pass stack variable as a ua argument, its value would be overwritten on object destruction! */
	err = ua_alloc(&app_ua, addr);
	if (err)
		goto out;

	/* Check UA is properly allocated */
	struct account *acc;
	acc = ua_account(app_ua);
	if (!acc) {
		warning("account: no account for this ua\n");
		goto out;
	}

	err = bevent_register(event_handler, 0);
	if (err)
		goto out;


	/* Execute any commands from input arguments */
	for (i=0; i<execmdc; i++) {
		ui_input_str(execmdv[i]);
	}

	if (tmo) {
		tmr_start(&tmr_quit, tmo * 1000, tmr_quit_handler, NULL);
	}

	/* Main loop */
	err = re_main(signal_handler);

 out:
	tmr_cancel(&tmr_quit);

	if (err)
		ua_stop_all(true);

	ua_close();

	/* note: must be done before mod_close() */
	module_app_unload();

	conf_close();

	baresip_close();

	/* NOTE: modules must be unloaded after all application
	 *       activity has stopped.
	 */
	debug("main: unloading modules..\n");
	mod_close();

	re_thread_async_close();

#ifdef RE_TRACE_ENABLED
	re_trace_close();
#endif

	/* Check for open timers */
	tmr_debug();

	libre_close();

	/* Check for memory leaks */
	mem_debug();

}

void StartBaresipTask(void)
{
    /* Create the main Task */
    if (xTaskCreate(baresip_main, "baresip_man", 1024*4, NULL, configMAX_PRIORITIES - 4, NULL) != pdPASS)
    {
        PRINTF("[!] MAIN Task creation failed!\r\n");
        while (1)
            ;
    }
}
