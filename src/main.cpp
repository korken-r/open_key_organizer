#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include <OneWire.h>
#include <globals.h>
#include <neo.h>
#include <helper.h>
#include <web.h>

const char *ap_ssid = "Oko";
const char *ap_pass = "";

const char *time_server = "pool.ntp.org";
const char *key_data_file = "oko_key_data.dat";
const char *config_file = "oko_config.json";

FS *fileSystem = &LittleFS;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, time_server);
OneWire ds(ONE_WIRE_PIN);

unsigned int NO_KEY[2] = {0, 0};
unsigned int BAD_CRC_KEY[2] = {0, 42};
uint16_t key_pos[MAX_KEYS] = {KEY_POS};
key_data kd[MAX_KEYS];
JsonDocument cfg; 
unsigned long config_create_time;
uint8_t task = NO;
bool has_wifi = false;
unsigned long last_blink=0,act_milis;
unsigned int blink_delay=100;

void set_id(unsigned int *a, unsigned int *b)
{
  a[0] = b[0];
  a[1] = b[1];
}

bool id_is_equal(unsigned int *a, unsigned int *b)
{
  if ((a[0] != b[0]) || (a[1] != b[1]))
    return false;
  return true;
}

bool id_is_no_key(unsigned int *a)
{
  if ((a[0] != NO_KEY[0]) || (a[1] != NO_KEY[1]))
    return false;
  return true;
}

int find_right_pos(unsigned int *a)
{
  for (int i = 0; i < MAX_KEYS; i++)
  {
    if (id_is_equal(a, kd[i].addr))
      return i;
  }
  return -1;
}

void save_config_data(String filename)
{
    File file = fileSystem->open(filename, "w");
    if (file)
    {
      serializeJson(cfg, file);
      file.close();
    }
}

void read_config_data(String filename)
{
    if (fileSystem->exists(filename))
    {
      String complete_data;
      Serial.print("Read config from: ");
      File file = fileSystem->open(filename, "r");
      if ( file.available()) {
        String data = file.readString();
        complete_data += data;
      }
      Serial.println(complete_data);
      file.close();
      deserializeJson(cfg, complete_data.c_str());  
    } else {
      cfg["ssid"] = "";
      cfg["pass"] = "";
      cfg["brightness"] = 100;
      cfg["blink_delay"] = 100;
    }
}

void save_key_data(String filename)
{
  if (has_wifi)
  {
    timeClient.begin();
    timeClient.update();
    config_create_time = timeClient.getEpochTime();
    timeClient.end();
  }
  else
  {
    config_create_time = 0;
  }

  File file = fileSystem->open(filename, "w");
  if (file)
  {
    file.write((unsigned char *)&config_create_time, sizeof(config_create_time));
    for (int i = 0; i < MAX_KEYS; i++)
    {
      file.write((unsigned char *)&i, sizeof(i));
      file.write((unsigned char *)kd[i].addr, 2 * sizeof(kd[i].addr[0]));
    }
    file.close();
  }
}

void read_key_data(String filename)
{
  if (fileSystem->exists(filename))
  {
    int tmp;
    Serial.print("Read key data from: ");
    File file = fileSystem->open(filename, "r");
    file.read((unsigned char *)&config_create_time, sizeof(config_create_time));
    Serial.println(date_time_from_epoch(config_create_time));
    for (int i = 0; i < MAX_KEYS; i++)
    {
      file.read((unsigned char *)&tmp, sizeof(tmp));
      file.read((unsigned char *)kd[i].addr, 2 * sizeof(kd[i].addr[0]));
      if (id_is_no_key(kd[i].addr))
      {
        kd[i].status = EMPTY;
      }
    }
    file.close();
  }
  else
  {
    Serial.println("No key data found, system needs to learn first.");
  }
}

// Modified version of shiftOut for pins conected via inverter
void myShiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
  uint8_t i;
  val = ~val;
  for (i = 0; i < 8; i++)
  {
    if (bitOrder == LSBFIRST)
      digitalWrite(dataPin, !!(val & (1 << i)));
    else
      digitalWrite(dataPin, !!(val & (1 << (7 - i))));

    digitalWrite(clockPin, LOW);
    delayMicroseconds(CLOCK_HIGH_DELAY);
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(CLOCK_LOW_DELAY);
  }
}

void set_out_address(byte value)
{
  // First three bites are adress, 8, 16 and 32 are inverted enable bits for
  // one of the three 4051
  if (value > 15)
  {
    value = value - 16;
    value = value | 24;
  }
  else if (value > 7)
  {
    value = value - 8;
    value = value | 40;
  }
  else
  {
    value = value | 48;
  }

#ifdef INVERT_LOGIC
  digitalWrite(LATCH_PIN, HIGH);
  digitalWrite(CLOCK_PIN, HIGH);
  myShiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, value);
  digitalWrite(LATCH_PIN, LOW);
#else
  digitalWrite(LATCH_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, value);
  digitalWrite(LATCH_PIN, HIGH);
#endif
  delay(SETTLING_TIME);
}

void read_id(int nmb, unsigned int *addr)
{
  bool result;
  set_out_address(key_pos[nmb]);
  result = ds.search((byte *)addr);
  ds.reset_search();
  if (result)
  {
    if (OneWire::crc8((byte *)addr, 7) != ((byte *)addr)[7])
    {
      set_id(addr, BAD_CRC_KEY);
    }
  }
  else
  {
    set_id(addr, NO_KEY);
  }
}

void perform_learning()
{
  colorAll(MAGENTA);
  Serial.println("Learn..");
  for (int i = 0; i < MAX_KEYS; i++)
  {
    read_id(i, kd[i].addr);
    print_ID(i, kd[i].addr);
    if (id_is_no_key(kd[i].addr))
      kd[i].status = EMPTY;
    else
      kd[i].status = IN;
  }
  save_key_data(key_data_file);
  Serial.println("Done..");
  send_key_status();
}

void check_keys()
{
  int other = -1;
  unsigned int addr[2];
  for (int i = 0; i < MAX_KEYS; i++)
  {
     kd[i].status = EMPTY;
  }
  for (int i = 0; i < MAX_KEYS; i++)
  {
    read_id(i, addr);
    if (id_is_equal(addr, kd[i].addr))
    {
      if (!id_is_no_key(kd[i].addr) && (kd[i].status != WRONG))
      {
        kd[i].status = IN;
      }
    }
    else if (id_is_no_key(addr) && (kd[i].status != WRONG))
    {
      kd[i].status = OUT;
    }
    else if (!id_is_equal(addr, kd[i].addr) && !id_is_no_key(addr))
    {
      kd[i].status = WRONG;
      other = find_right_pos(addr);
      if (other >= 0)
      {
        kd[other].status = WRONG;
        kd[other].blink = !kd[i].blink;
      }
    }
  }
}

void execute_web_tasks()
{
  switch (task)
  {
  case LEARN:
    perform_learning();
    break;
  case STATUS:
    send_key_status();
    break;
  case CONFIG:
    update_brightness( cfg["brightness"] );
    blink_delay = cfg["blink_delay"];
    save_config_data(config_file);
    break;
  case RESET:
    ESP.reset();
  }
  task = NO;
}

void setup(void)
{

  Serial.begin(115200);
  Serial.println("");
  Serial.println("Welcome to OKO");

  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
#ifdef INVERT_LOGIC
  digitalWrite(DATA_PIN, HIGH);
  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(LATCH_PIN, HIGH);
#else
  digitalWrite(DATA_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(LATCH_PIN, LOW);
#endif
  
  set_out_address(0);

  // internal LED seems do be inverted
  digitalWrite(LED_BUILTIN, HIGH);

  fileSystem->begin();
  read_key_data(key_data_file);
  read_config_data(config_file);
  blink_delay = cfg["blink_delay"];
  update_brightness( cfg["brightness"] );

  startNeo(WHITE);

  WiFi.mode(WIFI_STA);
  WiFi.begin((const char*)cfg["ssid"],(const char*)cfg["pass"]);

  // Wait for connection
  int max_tries = WIFI_MAX_WAIT * 2;
  while ((WiFi.status() != WL_CONNECTED) && (max_tries > 0))
  {
    delay(500);
    max_tries -= 1;
    Serial.print(".");
  }

  if (max_tries == 0)
  {
    WiFi.disconnect();
    Serial.print("Could not connect to ");
    Serial.println((const char*)cfg["ssid"]);
    // Switch on builtin LED to signal no WIFI
    digitalWrite(LED_BUILTIN, LOW);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_pass);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
    colorAll(MAGENTA);
    delay(1000);  
  }
  else
  {
    has_wifi = true;
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println((const char*)cfg["ssid"]);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  init_web(&task, kd, &config_create_time,&cfg);
  Serial.println("");
}

void loop(void)
{
  handle_web();
  execute_web_tasks();
  check_keys();
  
  act_milis = millis();
  if ( (act_milis-last_blink) >= blink_delay)
  {
    update_LEDs(kd,1);
    last_blink = act_milis;
  } else { 
    update_LEDs(kd,0);
  }
  //delay(3000);  
}