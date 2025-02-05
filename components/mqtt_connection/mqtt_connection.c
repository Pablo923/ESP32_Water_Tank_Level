#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "config.h"
#include "mqtt_client.h"

#define MQTT_BROKER_URL "mqtt://mqtt.eclipseprojects.io"

static const char *TAG = "MQTT_CLIENT";
static esp_mqtt_client_handle_t client;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static esp_err_t mqtt_client_init_internal(void);

esp_err_t mqtt_client_init(void)
{
    return mqtt_client_init_internal();
}

esp_err_t mqtt_client_start(void)
{
    return esp_mqtt_client_start(client);
}

esp_err_t mqtt_client_publish(const char *topic, const char *data, int len, int qos, int retain)
{
    return esp_mqtt_client_publish(client, topic, data, len, qos, retain) == -1 ? ESP_FAIL : ESP_OK;
}

esp_err_t mqtt_client_subscribe(const char *topic, int qos)
{
    return esp_mqtt_client_subscribe(client, topic, qos) == -1 ? ESP_FAIL : ESP_OK;
}

// Chapter: MQTT Client Internal Functions

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static esp_err_t mqtt_client_init_internal(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://mqtt.eclipseprojects.io",
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    return esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
}