#ifndef GLOBALS_H
#define GLOBALS_H

// maximum number of keys depends on hardware. With three 4051, max is 24
#define MAX_KEYS 22
#define LED_PER_KEY 2

#define LED_POS 1,2,5,6,9,10,13,14,17,18,21,0,3,4,7,8,11,12,15,16,19,20
#define KEY_POS 16,17,18,19,20,12,13,14,15,0,1,21,8,9,10,11,2,3,4,5,6,7

//#define LED_POS 0,1,2,3,4,5,6,7
//#define KEY_POS LED_POS

// adapt used pins here, depending on esp flavor
#if defined(ARDUINO_ESP8266_ADAFRUIT_HUZZAH)
#define DATA_PIN     14
#define CLOCK_PIN    12
#define LATCH_PIN    13
#define ONE_WIRE_PIN  2
#define NEO_PIN       5
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#define DATA_PIN     D1
#define CLOCK_PIN    D2
#define LATCH_PIN    D3
#define ONE_WIRE_PIN D6
#define NEO_PIN      D5
#endif

#define INVERT_LOGIC
#define INVERT_NEO

//mircoseconds
#define CLOCK_HIGH_DELAY 2
#define CLOCK_LOW_DELAY 0

//milliseconds
#define SETTLING_TIME 1

// nmb of seconds to wait for WIFI, after that, start without
#define WIFI_MAX_WAIT 10

#define NO      0
#define LEARN   1
#define CONFIG  2
#define STATUS  3
#define RESET   4
#define LEDTEST 5

#define IN       0
#define OUT      1 
#define WRONG    2
#define EMPTY    3

struct key_data {
  unsigned int addr[2];
  unsigned int status;
  unsigned int blink;
};

#endif