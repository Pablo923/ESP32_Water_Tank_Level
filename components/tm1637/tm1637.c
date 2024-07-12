#include "tm1637.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TM1637_ADDR_AUTO  0x40
#define TM1637_ADDR_FIXED 0x44
#define TM1637_STARTADDR  0xc0

static void tm1637_start(tm1637_led_t* led);
static void tm1637_stop(tm1637_led_t* led);
static void tm1637_write_byte(tm1637_led_t* led, uint8_t byte);

static const uint8_t digit_table[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

tm1637_led_t* tm1637_init(gpio_num_t clk, gpio_num_t dio) {
    tm1637_led_t* led = malloc(sizeof(tm1637_led_t));
    led->clk = clk;
    led->dio = dio;

    gpio_set_direction(clk, GPIO_MODE_OUTPUT);
    gpio_set_direction(dio, GPIO_MODE_OUTPUT);
    gpio_set_level(clk, 1);
    gpio_set_level(dio, 1);

    return led;
}

void tm1637_set_brightness(tm1637_led_t* led, uint8_t brightness) {
    if (brightness > 7) brightness = 7;
    tm1637_start(led);
    tm1637_write_byte(led, 0x87 + brightness);
    tm1637_stop(led);
}

void tm1637_display_number(tm1637_led_t* led, uint16_t number) {
    uint8_t digits[4];
    for (int i = 3; i >= 0; i--) {
        digits[i] = digit_table[number % 10];
        number /= 10;
    }

    tm1637_start(led);
    tm1637_write_byte(led, TM1637_ADDR_AUTO);
    tm1637_stop(led);

    tm1637_start(led);
    tm1637_write_byte(led, TM1637_STARTADDR);
    for (int i = 0; i < 4; i++) {
        tm1637_write_byte(led, digits[i]);
    }
    tm1637_stop(led);
}

static void tm1637_start(tm1637_led_t* led) {
    gpio_set_level(led->dio, 0);
    vTaskDelay(1 / portTICK_PERIOD_MS);
}

static void tm1637_stop(tm1637_led_t* led) {
    gpio_set_level(led->clk, 0);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(led->dio, 0);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(led->clk, 1);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(led->dio, 1);
}

static void tm1637_write_byte(tm1637_led_t* led, uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        gpio_set_level(led->clk, 0);
        gpio_set_level(led->dio, byte & 0x01);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        gpio_set_level(led->clk, 1);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        byte >>= 1;
    }

    // Wait for ACK
    gpio_set_level(led->clk, 0);
    gpio_set_direction(led->dio, GPIO_MODE_INPUT);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(led->clk, 1);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(led->clk, 0);
    gpio_set_direction(led->dio, GPIO_MODE_OUTPUT);
}