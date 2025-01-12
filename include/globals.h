#ifndef GLOBALS_H
#define GLOBALS_H

// nmb of seconds to wait for WIFI, after that, start without
#define WIFI_MAX_WAIT 10

// maximum number of keys depends on hardware. With two 4067, max is 32
#define MAX_KEYS 5

// adapt used pins here. This is for feather ESP 8266
#define DATA_PIN     14
#define CLOCK_PIN    12
#define LATCH_PIN    13
#define ENABLE_1_PIN 15
#define ENABLE_2_PIN 16
#define ONE_WIRE_PIN  2
#define NEO_PIN       5

#define BLINK_DELAY 100

#define NO     0
#define LEARN  1
#define CONFIG 2
#define STATUS 3

#define IN       0
#define OUT      1 
#define WRONG    2

struct key_data{
  unsigned int addr[2];
  unsigned int status;
};

#endif