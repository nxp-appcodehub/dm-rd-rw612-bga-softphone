/*
 * Copyright 2022-2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "wpl.h"
#include "cred_flash_storage.h"
#include <string.h>
#include "wlan_wrpr.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define DEMO_WIFI_LABEL             "MyWifi"
#define CREDENTIALS_FILE            "wifi_cred.dat"

/*******************************************************************************
 * Variables
 ******************************************************************************/

volatile bool wlan_wrpr_connected = false;
char wlan_wrpr_ssid[WPL_WIFI_SSID_LENGTH + 1];
char wlan_wrpr_passphrase[WPL_WIFI_PASSWORD_LENGTH + 1];

/*******************************************************************************
 * Code
 ******************************************************************************/

int wlan_wrpr_init(wlan_status_cb cb)
{
    wpl_ret_t err = WPLRET_FAIL;
    char security[64];

    /* Initialize WIFI*/
    err = WPL_Init();
    if (err != WPLRET_SUCCESS) {
        return -1;
    }

    err = WPL_Start(cb);
    if (err != WPLRET_SUCCESS) {
        return -1;
    }

    /* Get the saved credentials */
    if (get_saved_wifi_credentials(CREDENTIALS_FILE, wlan_wrpr_ssid, wlan_wrpr_passphrase, security) != 0 || wlan_wrpr_ssid[0] == '\0') {
        wlan_wrpr_ssid[0] = 0;
        wlan_wrpr_passphrase[0] = 0;
    }

    return 0;
}

char * wlan_wrpr_scan(void)
{
   return WPL_Scan();
}

int wlan_wrpr_connect(const char *ssid, const char *password)
{
    wpl_ret_t err;
    err = WPL_AddNetwork(ssid, password, DEMO_WIFI_LABEL);

    if (err != WPLRET_SUCCESS) {
        return -1;
    }

    err = WPL_Join(DEMO_WIFI_LABEL);
    if (err != WPLRET_SUCCESS) {
        WPL_RemoveNetwork(DEMO_WIFI_LABEL);
        return -1;
    }

    wlan_wrpr_connected = true;

    /* Save credentials to flash */
    if (save_wifi_credentials(CREDENTIALS_FILE, (char *)ssid, (char *)password, "") != 0) {
        return -1;
    }

    strncpy(wlan_wrpr_ssid, ssid, WPL_WIFI_SSID_LENGTH + 1);
    strncpy(wlan_wrpr_passphrase, password, WPL_WIFI_PASSWORD_LENGTH + 1);

    return 0;
}

 int wlan_wrpr_connect_saved(void)
 {
    wpl_ret_t err;

    char security[64];

    if (wlan_wrpr_connected == true) {
        return 0;
    }

    if (get_saved_wifi_credentials(CREDENTIALS_FILE, wlan_wrpr_ssid, wlan_wrpr_passphrase, security) != 0 || wlan_wrpr_ssid[0] == '\0') {
        return -1;
    }

    err = WPL_AddNetwork(wlan_wrpr_ssid, wlan_wrpr_passphrase, DEMO_WIFI_LABEL);
    if (err != WPLRET_SUCCESS) {
        return -1;
    }

    err = WPL_Join(DEMO_WIFI_LABEL);
    if (err != WPLRET_SUCCESS) {
        WPL_RemoveNetwork(DEMO_WIFI_LABEL);
        return -1;
    }

    wlan_wrpr_connected = true;
    return 0;
 }

#define UAP_SSID        "rw612_softphone"
#define UAP_PASSWORD    "12345678"
#define UAP_CHANNEL     1

int wlan_wrpr_start_uap(void)
{
    wpl_ret_t err;

    err = WPL_Start_AP(UAP_SSID, UAP_PASSWORD, UAP_CHANNEL);
    if (err != WPLRET_SUCCESS)
    {
        return -1;
    }

    return 0;
}

int wlan_wrpr_stop_uap(void)
{
    if (WPL_Stop_AP() != WPLRET_SUCCESS)
    {
        return -1;
    }

    return 0;
}

int wlan_wrpr_disconnect(void)
{
    if (WPL_Leave() != WPLRET_SUCCESS) {
        return -1;
    }

    if (WPL_RemoveNetwork(DEMO_WIFI_LABEL) != WPLRET_SUCCESS) {
        return -1;
    }

    if(reset_saved_wifi_credentials(CREDENTIALS_FILE)) {
        return -1;
    }

    wlan_wrpr_connected = false;
    memset(wlan_wrpr_ssid, 0, WPL_WIFI_SSID_LENGTH + 1);
    memset(wlan_wrpr_passphrase, 0, WPL_WIFI_PASSWORD_LENGTH + 1);

    return 0;
}