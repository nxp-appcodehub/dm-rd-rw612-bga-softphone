/*
 * Copyright 2020-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "wlan.h"

#define WIFI_MAX_NETWORKS 25
#define WIFI_MAX_SSID_LEN (32 + 1)
#define MAX_OUTPUT_LENGTH (WIFI_MAX_NETWORKS * (WIFI_MAX_SSID_LEN + 2))

typedef struct
{
    char ssid[WIFI_MAX_SSID_LEN];
    int rssi;
    uint8_t channel;
} wifi_network_t;

int WIFI_ScanNetworks(void);

wifi_network_t *WIFI_GetNetworks(void);

int WIFI_GetNetworkCount(void);

bool WIFI_IsScanDone(void);

#endif
