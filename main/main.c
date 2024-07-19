#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "ultrasonic.h"
#include "tm1637.h"

#define TM1637_CLK_PIN  GPIO_NUM_18
#define TM1637_DIO_PIN  GPIO_NUM_19

#define ULTRASONIC_ECHO_PIN  GPIO_NUM_25
#define ULTRASONIC_TRIG_PIN  GPIO_NUM_26

#define TAG "WATERTANK_APP"

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing...");

    // Initialize HC-SR04 sensor
    ultrasonic_sensor_t sensor = {.trigger_pin = ULTRASONIC_TRIG_PIN, .echo_pin = ULTRASONIC_ECHO_PIN};
    ultrasonic_init(&sensor);

    // Initialize TM1637 display 
    tm1637_led_t *display = tm1637_init(TM1637_CLK_PIN, TM1637_DIO_PIN);
    tm1637_set_brightness(display, 7);

    while(1)
    {
        // Print distance in cm from ultrasonic sensor
        float distance = measure_distance(&sensor);
        // ESP_LOGI(TAG, "Distance: %.2f cm", distance);

        // Convert distance in level of water (lts)
        float water_level = distance_to_level(distance);
        ESP_LOGI(TAG, "Level of Water: %.2f lts", water_level);

        if (water_level >= 0 && water_level <= MAX_WATER_LEVEL)
        {
            // Display water level in liters on TM1637
            tm1637_display_number(display, (uint16_t)water_level);
        }
        else{
            ESP_LOGI(TAG, "Error Measuring Water Level\n");
            tm1637_set_lines(display);
        }

        // Delay for 5 second
        vTaskDelay(5000 / portTICK_PERIOD_MS); 
    }
}
