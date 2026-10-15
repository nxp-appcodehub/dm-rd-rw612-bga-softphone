/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

typedef void (*wlan_status_cb)(bool);

extern volatile bool wlan_wrpr_connected;
extern char wlan_wrpr_ssid[];
extern char wlan_wrpr_passphrase[];

/**
 * @brief  Init and start the Wi-Fi driver and register an application link state callback.
 *
 * @param  cb Function which will be called from WPL layer in order to
 *            notify upper application that Wi-Fi link is lost or re/established.
 *
 * @return 0 Wi-Fi driver started successfully.
 */
int wlan_wrpr_init(wlan_status_cb cb);

 /**
 * @brief  Scan for nearby Wi-Fi networks.
           Print available networks information and store them in an internal buffer in JSON fomrat.
           The returned buffer is dynamically allocated, caller is responsible for deallocation.
 *         wlan_wrpr_scan should be called only after WPL_Start was successfully performed.
 *
 * @return Pointer to buffer with scan results, NULL if error.
 */
 char * wlan_wrpr_scan(void);

 /**
 * @brief  Connect to a Wi-Fi network and save it in flash for future connections.
 *
 * @param  ssid Name of the STA network to be created.
 * @param  password Password of the STA network to be created.
 *
 * @return 0 Device joined the Wi-Fi network using its STA interface.
 */
 int wlan_wrpr_connect(const char *ssid, const char *password);

 /**
 * @brief  Connect to a Wi-Fi network and save it in flash for future connections.
 *
 * @param  ssid Name of the STA network to be created.
 * @param  password Password of the STA network to be created.
 *
 * @return 0 Device joined the Wi-Fi network using its STA interface.
 */
 int wlan_wrpr_connect_saved(void);

/**
 * @brief  Disconnect from a Wi-Fi network and remove from flash for future connections.
 *
 * @return 0 Device disconnected from the Wi-Fi network.
 */
int wlan_wrpr_disconnect(void);

/**
 * @brief  Start the micro access point (UAP) with the built-in SSID "rw612_softphone"
 *         and password "12345678" on channel 1.
 *         Internally calls WPL_Start_AP and starts a DHCP server for connected clients.
 *         Must be called after wlan_wrpr_init().
 *
 * @return 0 UAP started successfully, -1 on error.
 */
int wlan_wrpr_start_uap(void);

/**
 * @brief  Stop the micro access point (UAP) started by wlan_wrpr_start_uap().
 *
 * @return 0 UAP stopped successfully, -1 on error.
 */
int wlan_wrpr_stop_uap(void);
