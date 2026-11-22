#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "ultrasonic.h"
#include "tm1637.h"
#include <string.h>
// #include "wifi.h"
// #include "mqtt_client.h"
// #include "mqtt_connection.h"
#include "RAK3172.h"
#include "dht11.h"
#include "UART.h"

#define TM1637_CLK_PIN       GPIO_NUM_18
#define TM1637_DIO_PIN       GPIO_NUM_19
#define ULTRASONIC_ECHO_PIN  GPIO_NUM_25
#define ULTRASONIC_TRIG_PIN  GPIO_NUM_26
#define DHT11_DATA_PIN       GPIO_NUM_27

// LoRa payload (see README): ID, level float32, temperature int16, humidity uint16, little-endian.
#define DEVICE_ID               0x01
#define LORA_PAYLOAD_SIZE       9
#define TEMPERATURE_NO_DATA     INT16_MIN   // 0x8000
#define HUMIDITY_NO_DATA        UINT16_MAX  // 0xFFFF

// #define EXAMPLE_ESP_WIFI_SSID   ""
// #define EXAMPLE_ESP_WIFI_PASS   ""

#define TAG "WATERTANK_APP"

static void build_lora_payload(uint8_t payload[LORA_PAYLOAD_SIZE], float water_level,
                               int16_t temperature_c100, uint16_t humidity_rh100)
{
    uint32_t level_bits;
    memcpy(&level_bits, &water_level, sizeof(level_bits));

    payload[0] = DEVICE_ID;
    payload[1] = level_bits & 0xFF;
    payload[2] = (level_bits >> 8) & 0xFF;
    payload[3] = (level_bits >> 16) & 0xFF;
    payload[4] = (level_bits >> 24) & 0xFF;
    payload[5] = (uint16_t)temperature_c100 & 0xFF;
    payload[6] = ((uint16_t)temperature_c100 >> 8) & 0xFF;
    payload[7] = humidity_rh100 & 0xFF;
    payload[8] = (humidity_rh100 >> 8) & 0xFF;
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing...");

    init_uart();
    init_RAK3172();

    // Initialize HC-SR04 sensor
    ultrasonic_sensor_t sensor = {.trigger_pin = ULTRASONIC_TRIG_PIN, .echo_pin = ULTRASONIC_ECHO_PIN};
    ultrasonic_init(&sensor);

    // Initialize TM1637 display
    tm1637_led_t display = {.clk_pin = TM1637_CLK_PIN, .dio_pin = TM1637_DIO_PIN};
    tm1637_init(&display);

    // Initialize DHT11 temperature/humidity sensor
    dht11_sensor_t dht11 = {.data_pin = DHT11_DATA_PIN};
    dht11_init(&dht11);

    // Create Task to Receive Message by UART
    xTaskCreate(uartSerial_receiveTask, "uartSerial_receiveTask", 2048, NULL, 10, NULL);
    // Create Task to Print RAK3172 Responses
    xTaskCreate(uartRAK3172_receiveTask, "uartRAK3172_receiveTask", 2048, NULL, 10, NULL);

    while(1)
    {
        // Print distance in cm from ultrasonic sensor
        float distance = measure_distance(&sensor);
        // Convert distance in level of water (lts)
        float water_level = distance_to_level(distance);

        if (water_level >= 0 && water_level <= MAX_WATER_LEVEL)
        {
            // Display water level in liters on TM1637
            tm1637_display_number(&display, (uint16_t)water_level);
            ESP_LOGI(TAG, "Level of Water: %.2f lts", water_level);

            int16_t temperature_c100 = TEMPERATURE_NO_DATA;
            uint16_t humidity_rh100 = HUMIDITY_NO_DATA;
            dht11_reading_t reading;
            esp_err_t dht11_err = dht11_read(&dht11, &reading);
            if (dht11_err == ESP_OK) {
                temperature_c100 = reading.temperature_c100;
                humidity_rh100 = reading.humidity_rh100;
                ESP_LOGI(TAG, "Temperature: %.2f C, Humidity: %.2f %%RH",
                         temperature_c100 / 100.0, humidity_rh100 / 100.0);
            } else {
                ESP_LOGW(TAG, "DHT11 read failed: %s", esp_err_to_name(dht11_err));
            }

            uint8_t payload[LORA_PAYLOAD_SIZE];
            build_lora_payload(payload, water_level, temperature_c100, humidity_rh100);

            char hex_payload[LORA_PAYLOAD_SIZE * 2 + 1];
            for (int i = 0; i < LORA_PAYLOAD_SIZE; i++) {
                snprintf(&hex_payload[i * 2], 3, "%02X", payload[i]);
            }
            ESP_LOGI(TAG, "LoRa payload: %s", hex_payload);

            // Formar el comando AT+PSEND
            char at_command[64];
            snprintf(at_command, sizeof(at_command), "AT+PSEND=%s", hex_payload);

            // Enviar comando al RAK3172
            RAK3172_sendCommand(at_command);
        }
        else{
            ESP_LOGI(TAG, "Error Measuring Water Level\n");
            tm1637_set_lines(&display);
        }

        // Delay for 5 second
        vTaskDelay(5000 / portTICK_PERIOD_MS); 
    }
}
