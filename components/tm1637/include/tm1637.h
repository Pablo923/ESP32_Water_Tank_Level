#ifndef TM1637_H
#define TM1637_H

#define SEG_G (1 << 6)

#include "driver/gpio.h"

typedef struct {
    gpio_num_t clk_pin;
    gpio_num_t dio_pin;
} tm1637_led_t;

void tm1637_init(tm1637_led_t *led);
void tm1637_display_number(tm1637_led_t *led, int number);
void tm1637_set_segment(tm1637_led_t *led, uint8_t digit_index, uint8_t segments);
void tm1637_set_lines(tm1637_led_t *led);
void tm1637_set_brightness(tm1637_led_t* led, uint8_t brightness);

#endif // TM1637_H