#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "ultrasonic.h"
#include "tm1637.h"
#include "wifi.h"
#include "mqtt_client.h"
#include "mqtt_connection.h"

#define TM1637_CLK_PIN       GPIO_NUM_18
#define TM1637_DIO_PIN       GPIO_NUM_19
#define ULTRASONIC_ECHO_PIN  GPIO_NUM_25
#define ULTRASONIC_TRIG_PIN  GPIO_NUM_26

#define EXAMPLE_ESP_WIFI_SSID   "***"
#define EXAMPLE_ESP_WIFI_PASS   "***"

#define TAG "WATERTANK_APP"

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing...");

    // Initialize Wifi
    wifi_connect(EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

    // Init MQTT client and Start
    mqtt_client_init();
    mqtt_client_start();

    // Initialize HC-SR04 sensor
    ultrasonic_sensor_t sensor = {.trigger_pin = ULTRASONIC_TRIG_PIN, .echo_pin = ULTRASONIC_ECHO_PIN};
    ultrasonic_init(&sensor);

    // Initialize TM1637 display
    tm1637_led_t display = {.clk_pin = TM1637_CLK_PIN, .dio_pin = TM1637_DIO_PIN};
    tm1637_init(&display);

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

            // Publish to MQTT
            char mqtt_data[20];
            snprintf(mqtt_data, sizeof(mqtt_data), "%.2f", water_level);
            mqtt_client_publish("/ESP32_Estanque_Casa/Nivel", mqtt_data, 0, 1, 1);
        }
        else{
            ESP_LOGI(TAG, "Error Measuring Water Level\n");
            tm1637_set_lines(&display);
        }

        // Delay for 5 second
        vTaskDelay(5000 / portTICK_PERIOD_MS); 
    }
}
