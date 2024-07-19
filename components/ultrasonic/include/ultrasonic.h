#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#define MAX_WATER_LEVEL 5400

#include "driver/gpio.h"

typedef struct {
    gpio_num_t trigger_pin;
    gpio_num_t echo_pin;
} ultrasonic_sensor_t;

void ultrasonic_init(ultrasonic_sensor_t *sensor);
float measure_distance(ultrasonic_sensor_t *sensor);
uint32_t distance_to_level(uint32_t distance);

#endif // ULTRASONIC_H