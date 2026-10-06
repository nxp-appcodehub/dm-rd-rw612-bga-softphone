# RW612 BGA Wi-Fi Softphone — Project Description
> Slide deck source material

---

## General Description

The **RW612 BGA Wi-Fi Softphone** is an embedded VoIP demo that turns the **NXP RD-RW612-BGA evaluation board** into a fully functional SIP softphone. It demonstrates end-to-end voice calling — from Wi-Fi provisioning and SIP registration, through encrypted audio streaming — entirely running on a bare-metal microcontroller environment using **FreeRTOS** and the **NXP MCUXpresso SDK**.

The demo is aimed at engineers evaluating the RW612's wireless, audio, and security capabilities in a real-world communications use case. It is production-style firmware: the SIP stack, TLS backend, audio drivers, and application shell are each cleanly separated and individually replaceable.

---

## Main Components

| Component | Description |
|---|---|
| **NXP RW612 SoC** | ARM Cortex-M33 with integrated Wi-Fi 6 (802.11ax), Bluetooth 5.3, and 802.15.4 — the target hardware platform. |
| **FreeRTOS** | Real-time operating system providing task scheduling, synchronization, and heap management for all firmware subsystems. |
| **Baresip SIP Stack** | Open-source, portable SIP User-Agent (RFC 3261) ported to FreeRTOS. Provides the full SIP signalling engine: REGISTER, INVITE, BYE, MESSAGE, and in-dialog request handling. Manages User-Agent (UA) lifecycle, call state machines, codec negotiation (SDP offer/answer), and audio session setup. Designed to be modular — codecs, transport, and audio I/O are all plug-in modules. Version 4.2.0. License: **BSD 3-Clause**. Source: https://github.com/baresip/baresip |
| **libre (re)** | Low-level real-time communications library that underpins Baresip. Provides: SIP message parsing and transport (UDP, TCP, TLS), SDP, DNS resolver, RTP/RTCP, STUN/TURN/ICE, a portable async I/O event loop, memory management, and the threading model used to integrate Baresip with FreeRTOS (`re_thread_enter` / `re_thread_leave`). Version 4.2.0. License: **BSD 3-Clause**. Source: https://github.com/baresip/re |
| **MbedTLS 3.x** | TLS 1.3 backend providing encrypted SIP transport (SIPS, port 5061) and SRTP media encryption. Supports optional mutual TLS (mTLS) client certificate verification. |
| **NXP Wi-Fi Driver (WPL)** | Wi-Fi Provisioning Layer managing 2.4 GHz / 5 GHz network association. Supports both station mode and access-point provisioning mode. |
| **NXP Audio Streamer (Maestro)** | NXP Maestro audio framework providing the PCM pipeline between hardware and the SIP media path. Runs two independent streamer pipelines: one for microphone capture (`STREAMER_mic_Create`) and one for audio playback (`STREAMER_play_Create`). Interfaces with the RW612 audio subsystem over I2S. The `nxp_audio` Baresip module bridges the Maestro streamer to Baresip's audio source/sink API via registered callbacks (`baresip_ausrc_callback_h` / `baresip_auplay_callback_h`). |
| **G.711 Audio Codec** | ITU-T standard PCM codec implemented as a Baresip plug-in module (`baresip/modules/g711`). Supports both µ-law (PCMU, payload type 0) and A-law (PCMA, payload type 8) variants. Operates at 8 kHz, 8-bit samples, 64 kbps — the universal baseline codec for SIP/RTP interoperability with any SIP server or phone. No external library dependency; codec logic is self-contained. |
| **Opus Audio Codec** | Modern, highly adaptive audio codec (RFC 6716) implemented as a Baresip plug-in module (`baresip/modules/opus`). Configured in VOIP application mode at 16 kHz mono with a maximum average bitrate of 24 kbps. Features enabled: in-band Forward Error Correction (FEC) and Discontinuous Transmission (DTX) for improved resilience on lossy Wi-Fi links. SDP negotiation (offer/answer) is handled by `sdp.c`. Requires the `libopus` library (v1.0.0 or later). |
| **LVGL GUI** | Lightweight graphics library providing a touch-screen graphical user interface running as a dedicated FreeRTOS task. |
| **UART Shell (CLI)** | MCUXpresso SDK shell module exposing Wi-Fi and SIP commands over a 115200-baud serial console. |
| **Flash Credential Storage** | Persistent key-value store that saves Wi-Fi credentials to on-chip flash, enabling automatic reconnection after power-cycle. |
| **Certificates (PKI)** | CA, server, and optional client certificates compiled into the firmware as C header arrays (DER format), supporting both a built-in test CA and a user-supplied private CA. |

---

## Main Functionality

### 1. Wi-Fi Connectivity & Provisioning
- Scan for available Wi-Fi networks from the serial console.
- Connect to a network (open or WPA2) and automatically save credentials to flash.
- Reconnect to the saved network on subsequent boots with a single command.
- Link-state monitoring with console notification on link loss or re-establishment.

### 2. SIP Registration
- On command (`sip_start`), the Baresip User-Agent registers with a configured SIP server.
- Supports both **plain UDP** (development) and **TLS 1.3 / SIPS** (secure production) transport.
- SIP account credentials (username, password, server address) are set at compile time.

### 3. Voice Calls
- **Outgoing calls** — dial any SIP URI from the shell (`sip_call sip:1001@server`).
- **Incoming calls** — notified on the console; answered or rejected via `sip_answer` / `sip_hangup`.
- Audio codecs supported: **G.711 µ-law / a-law** and **Opus**.
- Full-duplex audio path: on-board microphone → RTP/SRTP → network → speaker.

### 4. Secure Media (SRTP)
- When `SERVER_MODE` is enabled, all media is encrypted using **SRTP**.
- TLS 1.3 secures the SIP signalling channel.
- Optional **mutual TLS (mTLS)** allows the server to authenticate the board's client certificate.

### 5. SIP Instant Messaging
- Send and receive SIP MESSAGE requests (`sip_send_msg`).
- Incoming messages are printed to the console in real time.

### 6. Graphical User Interface (GUI)
- An LVGL-based touch-screen UI runs in parallel as a dedicated FreeRTOS task.
- UI layout and events are generated by the **NXP GUI Guider** tool.

### 7. UART Command-Line Interface
- Full serial shell for all Wi-Fi and SIP operations.
- Debug command (`task_stats`) reports FreeRTOS CPU usage, stack high-water marks, and free heap.

---

## Key Technical Highlights

- **Single-chip VoIP** — all processing (networking, TLS, SIP, audio, UI) on one ARM Cortex-M33.
- **Zero file system** — certificates and configuration are compiled directly into flash as C arrays.
- **Layered architecture** — `softphone.c` API cleanly decouples the application layer from Baresip internals.
- **Build system** — CMake + Ninja with preset-based builds (`flash_debug` / `flash_release`); compatible with MCUXpresso for VS Code.
- **Open-source foundation** — Baresip, libre/rem, MbedTLS, LwIP, FreeRTOS, and LVGL — all integrated under BSD/Apache-compatible licenses.
