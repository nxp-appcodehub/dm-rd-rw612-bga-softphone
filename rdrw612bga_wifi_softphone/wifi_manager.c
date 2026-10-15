/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "wifi_manager.h"

#include "wpl.h"
#include "fsl_debug_console.h"

static wifi_network_t s_networks[WIFI_MAX_NETWORKS];
static int s_networkCount = 0;
static volatile bool s_scanDone = false;

static int wifi_scan_callback(unsigned int count)
{
    struct wlan_scan_result result;
    int ret;

    s_networkCount = 0;

    for (uint32_t i = 0; i < count; i++)
    {
        ret = wlan_get_scan_result(i, &result);

        if (ret == WM_SUCCESS)
        {
            if (s_networkCount < WIFI_MAX_NETWORKS)
            {
                strncpy(s_networks[s_networkCount].ssid,
                        result.ssid,
                        WIFI_MAX_SSID_LEN - 1);

                s_networks[s_networkCount]
                    .ssid[WIFI_MAX_SSID_LEN - 1] = '\0';

                s_networks[s_networkCount].rssi =
                    -(int)result.rssi;

                s_networks[s_networkCount].channel =
                    result.channel;

                s_networkCount++;
            }
        }
    }
    s_scanDone = true;

    return WM_SUCCESS;
}

int WIFI_ScanNetworks(void)
{
    s_scanDone = false;
    return wlan_scan(wifi_scan_callback);
}

wifi_network_t *WIFI_GetNetworks(void)
{
    return s_networks;
}

int WIFI_GetNetworkCount(void)
{
    return s_networkCount;
}


bool WIFI_IsScanDone(void)
{
    return s_scanDone;
}
