#include <Arduino.h>
#include <time.h>
#include <timeLib.h>
#include <helper.h>

String format_ID(unsigned int *addr)
{
  String out;
  byte* as_bytes = (byte*)addr;
  for (int i = 0; i < 7; i++)
  {
    out += String(as_bytes[i], HEX) + " ";
  }
  return out;
}

void print_ID(int nmb, unsigned int *addr)
{
  Serial.print(nmb);
  Serial.print(" ID = ");
  Serial.print(format_ID(addr));
  Serial.println("");
}

String date_time_from_epoch(unsigned long timestamp)
{
  String d = String(day(timestamp));
  if (d.length() == 1)
    d = "0" + d;
  String M = String(month(timestamp));
  if (M.length() == 1)
    M = "0" + M;
  String h = String(hour(timestamp));
  if (h.length() == 1)
    h = "0" + h; 
  String m = String(minute(timestamp));
  if (m.length() == 1)
    m = "0" + m; 
  String dt = d + "." + 
              M + "." + 
              String(year(timestamp)) + " " + 
              h + ":" + m;
  return dt;
}