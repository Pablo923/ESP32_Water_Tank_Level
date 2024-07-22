#include "tm1637.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TM1637_ADDR_AUTO 0x40
#define TM1637_ADDR_FIXED 0x44
#define TM1637_START_ADDR 0xC0

static const uint8_t digit_table[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static void tm1637_start(tm1637_led_t *led);
static void tm1637_stop(tm1637_led_t *led);
static void tm1637_write_byte(tm1637_led_t *led, uint8_t byte);

void tm1637_init(tm1637_led_t *led)
{
    gpio_set_direction(led->clk_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(led->dio_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(led->clk_pin, 1);
    gpio_set_level(led->dio_pin, 1);
    tm1637_set_brightness(led, 7);
}

void tm1637_display_number(tm1637_led_t *led, int number)
{
    uint8_t digits[4];
    for (int i = 3; i >= 0; i--) {
        digits[i] = digit_table[number % 10];
        number /= 10;
    }

    tm1637_start(led);
    tm1637_write_byte(led, TM1637_ADDR_AUTO);
    tm1637_stop(led);

    tm1637_start(led);
    tm1637_write_byte(led, TM1637_START_ADDR);
    for (int i = 0; i < 4; i++) {
        tm1637_write_byte(led, digits[i]);
    }
    tm1637_stop(led);
}

void tm1637_set_segment(tm1637_led_t *led, uint8_t digit_index, uint8_t segments)
{
    tm1637_start(led);
    tm1637_write_byte(led, TM1637_ADDR_FIXED);
    tm1637_stop(led);

    tm1637_start(led);
    tm1637_write_byte(led, digit_index | 0xC0);
    tm1637_write_byte(led, segments);
    tm1637_stop(led);
}

void tm1637_set_lines(tm1637_led_t *led)
{
    tm1637_set_segment(led, 0, SEG_G);
    tm1637_set_segment(led, 1, SEG_G);
    tm1637_set_segment(led, 2, SEG_G);
    tm1637_set_segment(led, 3, SEG_G);
}

void tm1637_set_brightness(tm1637_led_t* led, uint8_t brightness)
{
    if (brightness > 7) brightness = 7;
    tm1637_start(led);
    tm1637_write_byte(led, 0x87 + brightness);
    tm1637_stop(led);
}

static void tm1637_start(tm1637_led_t *led)
{
    gpio_set_level(led->clk_pin, 1);
    gpio_set_level(led->dio_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(1));
    gpio_set_level(led->dio_pin, 0);
}

static void tm1637_stop(tm1637_led_t *led)
{
    gpio_set_level(led->clk_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
    gpio_set_level(led->dio_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
    gpio_set_level(led->clk_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(1));
    gpio_set_level(led->dio_pin, 1);
}

static void tm1637_write_byte(tm1637_led_t *led, uint8_t byte)
{
    for (int i = 0; i < 8; i++) {
        gpio_set_level(led->clk_pin, 0);
        gpio_set_level(led->dio_pin, byte & 0x01);
        vTaskDelay(pdMS_TO_TICKS(1));
        gpio_set_level(led->clk_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(1));
        byte >>= 1;
    }

    gpio_set_level(led->clk_pin, 0);
    gpio_set_level(led->dio_pin, 1);
    gpio_set_level(led->clk_pin, 1);
    gpio_set_direction(led->dio_pin, GPIO_MODE_INPUT);
    vTaskDelay(pdMS_TO_TICKS(1));
    uint8_t ack = gpio_get_level(led->dio_pin);
    gpio_set_direction(led->dio_pin, GPIO_MODE_OUTPUT);
}