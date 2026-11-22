#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>
#include "driver/gpio.h"
#include "esp_err.h"

typedef struct {
    gpio_num_t data_pin;
} dht11_sensor_t;

typedef struct {
    int16_t temperature_c100;   // Hundredths of °C (2350 = 23.50 °C)
    uint16_t humidity_rh100;    // Hundredths of %RH (5400 = 54.00 %RH)
} dht11_reading_t;

void dht11_init(dht11_sensor_t *sensor);

// Blocks for about 25 ms. The DHT11 needs at least 1 s between reads.
// Returns ESP_ERR_TIMEOUT if the sensor does not answer and
// ESP_ERR_INVALID_CRC if the checksum does not match.
esp_err_t dht11_read(dht11_sensor_t *sensor, dht11_reading_t *reading);

#endif // DHT11_H
