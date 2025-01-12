#include <Adafruit_NeoPixel.h>
#include <neo.h>
#include <globals.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_KEYS, NEO_PIN, NEO_GRB + NEO_KHZ800);

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
  for (uint16_t i = 0; i < MAX_KEYS; i++)
  {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void colorOne(uint32_t i, uint32_t c)
{
  if (c < MAXC)
    c = fix_colors[c];
  strip.setPixelColor(i, c);
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
  for (int i = 0; i < MAX_KEYS; i++)
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