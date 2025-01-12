#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <globals.h>
#include <helper.h>

FS *l_fs = &LittleFS;

ESP8266WebServer server(80);

uint8_t *g_task;
key_data *g_kd;
unsigned long *g_learn_date;

void sendFile(String path, String contentType)
{
  if (l_fs->exists(path))
  {
    Serial.println(path);
    File file = l_fs->open(path, "r");
    size_t send = server.streamFile(file, contentType);
    if (send != file.size())
    {
      Serial.println("Sent less data than expected! " + String(send) + " of " + String(file.size()));
    }
    file.close();
  }
  else
  {
    Serial.println("could not find ");
  }
}

void handleRoot()
{
  sendFile("/index.html", "text/html");
}

void handleSet()
{
  sendFile("/set.html", "text/html");
}

void handleImg()
{
  sendFile("/oko.png", "image/png");
}

void handleCss()
{
  sendFile("/oko.css", "text/css");
}

void handleRequest()
{
  if (server.args() > 0)
  {
    for (uint8_t i = 0; i < server.args(); i++)
    {
      if (server.argName(i).compareTo("press") == 0)
      {
        *g_task = strtol(server.arg(i).c_str(), NULL, 10);
      }
    }
    Serial.println("pressed: " + String(*g_task));
  }
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void send_status()
{
  JsonDocument json;
  char output[512];
  json["info"] = date_time_from_epoch(*g_learn_date);
  JsonArray keys = json["keys"].to<JsonArray>();
  JsonArray status = json["status"].to<JsonArray>();
  for (int i = 0; i < MAX_KEYS; i++)
  {
    keys.add(format_ID(g_kd[i].addr));
    status.add(g_kd[i].status);
  }
  serializeJson(json, output);
  server.send(200, "application/json", output);
  //Serial.println(output);
}

void init_web(uint8_t *task_ptr,key_data *kd_ptr, unsigned long *learn_date_ptr)
{
    g_task = task_ptr;
    g_kd = kd_ptr;
    g_learn_date = learn_date_ptr;

    server.on("/", handleRoot);
    server.on("/set.html", handleSet);
    server.on("/request", handleRequest);
    server.on("/oko.png", handleImg);
    server.on("/oko.css", handleCss);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");
}

void handle_web()
{
    server.handleClient();
}