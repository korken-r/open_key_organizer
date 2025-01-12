#ifndef HELPER_H
#define HELPER_H
#include <Arduino.h>

String format_ID(unsigned int *addr);
void print_ID(int nmb, unsigned int *addr);
String date_time_from_epoch(unsigned long timestamp);

#endif