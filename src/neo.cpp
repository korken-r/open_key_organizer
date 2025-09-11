#include <Adafruit_NeoPixel.h>
#include <neo.h>
#include <globals.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_KEYS * LED_PER_KEY, NEO_PIN, NEO_GRB + NEO_KHZ800);

uint16_t led_pos[] = {0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46};

uint32_t fix_colors[MAXC] = {
    strip.Color(0, 0, 0),
    strip.Color(255, 255, 255),
    strip.Color(255, 0, 0),
    strip.Color(0, 255, 0),
    strip.Color(0, 0, 255),
    strip.Color(255, 0, 255)};

void colorAll(uint32_t c)
{
  if (c < MAXC)
    c = fix_colors[c];
  for (uint16_t i = 0; i < MAX_KEYS * LED_PER_KEY; i++)
  {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void colorOne(uint16_t i, uint32_t c)
{
  if (c < MAXC)
    c = fix_colors[c];
  uint16_t pos = led_pos[i];

  for (uint16_t j = 0; j < LED_PER_KEY; j++)
    strip.setPixelColor(pos + j, c);
}

void show_LEDs()
{
  strip.show();
}
void allOff()
{
  colorAll(0);
}

void startNeo(uint32_t initial_color)
{
  strip.begin();
  strip.setBrightness(100);
  colorAll(initial_color);
  strip.show();
}

void update_LEDs(key_data *kd)
{
  static int toggle = 1;
  static unsigned long last_toggle = 0;
  unsigned long now = 0;
  for (uint16_t i = 0; i < MAX_KEYS; i++)
  {
    if (kd[i].status == IN)
      colorOne(i, GREEN);
    else if (kd[i].status == OUT)
      colorOne(i, MAGENTA);
    else
    { 
      if (toggle)
      {
        colorOne(i, RED);
        toggle = 0;
      } else {
        colorOne(i, BLACK);
        toggle = 1;
      }
    }
  }
  now = millis();
  if ((now-last_toggle) >= BLINK_DELAY)
  {
    toggle = !toggle;
    last_toggle = now;
  }  
  show_LEDs();
}