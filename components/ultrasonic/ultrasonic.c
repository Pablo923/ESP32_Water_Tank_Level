#include "ultrasonic.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SOUND_SPEED_CM_US 0.0343

void ultrasonic_init(ultrasonic_sensor_t *sensor)
{
    gpio_set_direction(sensor->trigger_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(sensor->echo_pin, GPIO_MODE_INPUT);
}

float measure_distance(ultrasonic_sensor_t *sensor)
{
    gpio_set_level(sensor->trigger_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(2));
    gpio_set_level(sensor->trigger_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(sensor->trigger_pin, 0);

    int64_t start = esp_timer_get_time();
    while (gpio_get_level(sensor->echo_pin) == 0 && esp_timer_get_time() - start < 1000000);
    start = esp_timer_get_time();

    int64_t end = esp_timer_get_time();
    while (gpio_get_level(sensor->echo_pin) == 1 && esp_timer_get_time() - end < 1000000);
    end = esp_timer_get_time();

    float distance = (end - start) * SOUND_SPEED_CM_US / 2;
    return distance;
} 