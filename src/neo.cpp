#include <Adafruit_NeoPixel.h>
#include <neo.h>
#include <globals.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_KEYS * LED_PER_KEY, NEO_PIN, NEO_GRB + NEO_KHZ800);

uint8_t brightness = 100;

uint16_t led_pos[MAX_KEYS] = {LED_POS};

uint32_t fix_colors[MAXC] = {
    strip.Color(0, 0, 0),
    strip.Color(255, 255, 255),
    strip.Color(255, 0, 0),
    strip.Color(0, 255, 0),
    strip.Color(0, 0, 255),
    strip.Color(255, 0, 255),
    strip.Color(255, 255, 0)};
// WLEDs are also connected via inverter HC14. Do invert and brightness here.
// Take care: Do not use setBrightness from Adafruit_NeoPixel lib!
void setPixelColorInvert(uint16_t n, uint32_t c)
{
  if (c < MAXC)
    c = fix_colors[c];
  uint8_t r = (uint8_t)(c >> 16), g = (uint8_t)(c >> 8), b = (uint8_t)c;
  r = (r * brightness) >> 8;
  g = (g * brightness) >> 8;
  b = (b * brightness) >> 8;
  c = ((uint32_t)r << 16) + ((uint32_t)g << 8) + (uint32_t)b;
  uint32_t c_inv = ~c;
  strip.setPixelColor(n, c_inv);
}

void colorAll(uint32_t c)
{
  for (uint16_t i = 0; i < (MAX_KEYS * LED_PER_KEY); i++)
  {
    setPixelColorInvert(i, c);
  }
  strip.show();
}

void colorOne(uint16_t i, uint32_t c)
{
  uint16_t pos = led_pos[i] * LED_PER_KEY;

  for (uint16_t j = 0; j < LED_PER_KEY; j++)
  {
    setPixelColorInvert(pos + j, c);
  }
}

void start_animation(uint32_t bc,uint32_t fc)
{
  for (uint16_t i = 0; i < MAX_KEYS; i++)
  {
    colorOne(i,fc);
    strip.show();
    delay(200);
    colorOne(i,bc);  
  }
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
  colorAll(initial_color);
  delay(2000);
  colorAll(RED);
  delay(2000);
  colorAll(GREEN);
  delay(2000);
  colorAll(MAGENTA);
  delay(2000);
  colorAll(YELLOW);
  delay(2000);
  colorAll(WHITE);
  delay(2000);
  start_animation(BLUE,RED);
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
      }
      else
      {
        colorOne(i, BLACK);
        toggle = 1;
      }
    }
  }
  now = millis();
  if ((now - last_toggle) >= BLINK_DELAY)
  {
    toggle = !toggle;
    last_toggle = now;
  }
  show_LEDs();
}