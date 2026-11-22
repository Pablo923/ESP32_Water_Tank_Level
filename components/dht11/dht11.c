#include "dht11.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DHT11_START_LOW_MS   20
#define DHT11_TIMEOUT_US     100
// A data bit is 26-28 us high for 0 and 70 us high for 1.
#define DHT11_BIT_ONE_US     40

static portMUX_TYPE dht11_mux = portMUX_INITIALIZER_UNLOCKED;

// Waits while the pin stays at `level`. Returns the time spent in us,
// or -1 if it exceeds the timeout.
static int wait_while_level(gpio_num_t pin, int level)
{
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(pin) == level) {
        if (esp_timer_get_time() - start > DHT11_TIMEOUT_US) {
            return -1;
        }
    }
    return (int)(esp_timer_get_time() - start);
}

void dht11_init(dht11_sensor_t *sensor)
{
    gpio_set_direction(sensor->data_pin, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(sensor->data_pin, GPIO_PULLUP_ONLY);
    gpio_set_level(sensor->data_pin, 1);
}

esp_err_t dht11_read(dht11_sensor_t *sensor, dht11_reading_t *reading)
{
    gpio_num_t pin = sensor->data_pin;
    uint8_t data[5] = {0};
    esp_err_t err = ESP_OK;

    // Start signal: hold the line low for at least 18 ms, then release it.
    gpio_set_level(pin, 0);
    vTaskDelay(pdMS_TO_TICKS(DHT11_START_LOW_MS));

    // The response is timed in microseconds, so keep interrupts off while reading it (about 5 ms).
    portENTER_CRITICAL(&dht11_mux);
    gpio_set_level(pin, 1);

    // Sensor answers: ~20-40 us high, 80 us low, 80 us high.
    if (wait_while_level(pin, 1) < 0 || wait_while_level(pin, 0) < 0 || wait_while_level(pin, 1) < 0) {
        err = ESP_ERR_TIMEOUT;
    }

    for (int i = 0; i < 40 && err == ESP_OK; i++) {
        int high_us;
        if (wait_while_level(pin, 0) < 0 || (high_us = wait_while_level(pin, 1)) < 0) {
            err = ESP_ERR_TIMEOUT;
            break;
        }
        data[i / 8] <<= 1;
        if (high_us > DHT11_BIT_ONE_US) {
            data[i / 8] |= 1;
        }
    }
    portEXIT_CRITICAL(&dht11_mux);

    if (err != ESP_OK) {
        return err;
    }
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4]) {
        return ESP_ERR_INVALID_CRC;
    }

    // data: humidity integer, humidity tenths, temperature integer, temperature tenths
    // (bit 7 of the temperature tenths byte is the sign), checksum.
    int temperature_c100 = data[2] * 100 + (data[3] & 0x7F) * 10;
    if (data[3] & 0x80) {
        temperature_c100 = -temperature_c100;
    }
    reading->temperature_c100 = (int16_t)temperature_c100;
    reading->humidity_rh100 = (uint16_t)(data[0] * 100 + data[1] * 10);
    return ESP_OK;
}
