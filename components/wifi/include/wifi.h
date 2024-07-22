// #ifndef WIFI_H
// #define WIFI_H

// #include <stdint.h>

// // #define EXAMPLE_ESP_WIFI_SSID      "YOUR_WIFI_SSID"
// // #define EXAMPLE_ESP_WIFI_PASS      "YOUR_WIFI_PASSWORD"
// // #define EXAMPLE_ESP_MAXIMUM_RETRY  5

// // static EventGroupHandle_t s_wifi_event_group;

// // #define WIFI_CONNECTED_BIT BIT0
// // #define WIFI_FAIL_BIT      BIT1

// // static const char *TAG = "wifi station";

// // static int s_retry_num = 0;

// void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
// void wifi_init_sta(void);

// #endif 


#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"

esp_err_t example_wifi_connect(const char* ssid, const char* password);

#endif // WIFI_CONNECT_H