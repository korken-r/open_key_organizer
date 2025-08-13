#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include <OneWire.h>
#include <globals.h>
#include <neo.h>
#include <helper.h>
#include <web.h>

const char *ssid = "";
const char *password = "";
const char *time_server = "pool.ntp.org";
const char *config_file = "oko_config.dat";

FS *fileSystem = &LittleFS;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,time_server);
OneWire ds(ONE_WIRE_PIN);

const unsigned int NO_KEY[2] = {0,0};
const unsigned int BAD_CRC_KEY[2] = {0,42};

key_data kd[MAX_KEYS];
unsigned long config_create_time;
uint8_t task = NO;
bool has_wifi = false;

bool id_is_equal(unsigned int *a, unsigned int *b)
{
   if ((a[0] != b[0]) || (a[1] != b[1])) 
      return false;
  return true;
}

int find_right_pos(unsigned int *a)
{
  for (int i = 0; i < MAX_KEYS; i++)
   {
      if (id_is_equal(a,kd[i].addr))
        return i;
   }
   return -1;
}

void save_key_data(String filename)
{
  if (has_wifi)
  {
    timeClient.begin();
    timeClient.update();
    config_create_time = timeClient.getEpochTime();
    timeClient.end();
  } else {
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
    Serial.print("Read config from: ");
    File file = fileSystem->open(filename, "r");
    file.read((unsigned char *)&config_create_time, sizeof(config_create_time));
    Serial.println(date_time_from_epoch(config_create_time)) ;
    for (int i = 0; i < MAX_KEYS; i++)
    {
      file.read((unsigned char *)&tmp, sizeof(tmp));
      file.read((unsigned char *)kd[i].addr, 2 * sizeof(kd[i].addr[0]));
    }
    file.close();
  }
  else
  {
    Serial.println("No config found, system needs to learn first.");
  }
}

void set_out_address(byte value)
{
  if (value > 15)
  {
    digitalWrite(ENABLE_1_PIN, HIGH);
    digitalWrite(ENABLE_2_PIN, LOW);
    value = value << 4;
  }
  else
  {
    digitalWrite(ENABLE_2_PIN, HIGH);
    digitalWrite(ENABLE_1_PIN, LOW);
  }

  digitalWrite(LATCH_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, value);
  digitalWrite(LATCH_PIN, HIGH);
  delay(5);
}

void read_id(int nmb, unsigned int *addr)
{
  bool result;
  set_out_address(nmb);
  result = ds.search((byte*) addr);
  ds.reset_search();
  if (result)
  {
    if (OneWire::crc8((byte*)addr, 7) != ((byte*)addr)[7])
    {
      memcpy(addr,BAD_CRC_KEY,8); 
    }
  }
  else
  {
    memcpy(addr,NO_KEY,8);
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
    kd[i].status = IN;
  }
  save_key_data(config_file);
  Serial.println("Done..");
  allOff();
  send_status();
}

void check_keys()
{
  int other = -1;
  unsigned int addr[2];
  for (int i = 0; i < MAX_KEYS; i++)
  {
    read_id(i, addr);
    if (id_is_equal(addr,kd[i].addr))
    {
      kd[i].status = IN;
    } else if ((addr[0] == 0) && (i != other))
    {
       kd[i].status = OUT;
    } else {        
        kd[i].status = WRONG;
        other = find_right_pos(addr);
        if (other >= 0)
        {
          kd[other].status = WRONG;   
        }   
    }
    delay(10);
    //print_ID(i,addr);
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
    send_status();
    break;
  }
  task = NO;
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println("");
  
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(ENABLE_1_PIN, OUTPUT);
  pinMode(ENABLE_2_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(DATA_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(LATCH_PIN, LOW);
  set_out_address(0);
  digitalWrite(ENABLE_1_PIN, HIGH);
  digitalWrite(ENABLE_2_PIN, HIGH);
  
  // internal LED seems do be inverted
  digitalWrite(LED_BUILTIN, HIGH);

  startNeo(BLUE);

  fileSystem->begin();
  read_key_data(config_file);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
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
    Serial.println("");
    Serial.print("Could not connect to ");
    Serial.println(ssid);
    // Switch on builtin LED to signal no WIFI
    digitalWrite(LED_BUILTIN, LOW);
    WiFi.mode(WIFI_OFF);
  }
  else
  {
    has_wifi = true;
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    init_web(&task,kd,&config_create_time);
  }
}

void loop(void)
{
  if (has_wifi)
  {
    handle_web();
    execute_web_tasks();
  }
  check_keys();
  update_LEDs(kd);
  //delay(1000);
}