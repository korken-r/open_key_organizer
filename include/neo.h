#ifndef NEO_H
#define NEO_H
#include <globals.h>

void startNeo(uint32_t initial_color);
void colorOne(uint32_t i, uint32_t c);
void colorAll(uint32_t c);
void show_LEDs();
void allOff();
void update_LEDs(key_data *kd);

#define BLACK   0
#define WHITE   1
#define RED     2
#define GREEN   3
#define BLUE    4
#define MAGENTA 5
#define MAXC    6

#define LINEAR   0
#define ZICKZACK 1 

#define LED_ORDER ZICKZACK


#endif