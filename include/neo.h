#ifndef NEO_H
#define NEO_H
#include <globals.h>

void startNeo(uint32_t initial_color);
void colorOne(uint32_t i, uint32_t c);
void colorAll(uint32_t c);
void update_LEDs(key_data *kd, int toggle);
void update_brightness(uint8_t value);
void do_led_test(uint8_t test_pattern);

#define BLACK   0
#define WHITE   1
#define RED     2
#define GREEN   3
#define BLUE    4
#define MAGENTA 5
#define YELLOW  6
#define MAXC    7

#endif