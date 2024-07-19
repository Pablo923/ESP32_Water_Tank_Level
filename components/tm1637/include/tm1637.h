#ifndef TM1637_H
#define TM1637_H

#include "driver/gpio.h"

#define SEG_G (1 << 6)

typedef struct {
    gpio_num_t clk;
    gpio_num_t dio;
} tm1637_led_t;

tm1637_led_t* tm1637_init(gpio_num_t clk, gpio_num_t dio);
void tm1637_set_brightness(tm1637_led_t* led, uint8_t brightness);
void tm1637_display_number(tm1637_led_t* led, uint16_t number);
void tm1637_set_segment(tm1637_led_t *led, uint8_t digit_index, uint8_t segments);
void tm1637_set_lines(tm1637_led_t *led);

#endif // TM1637_H