#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

#ifndef __NTP_H__
#define __NTP_H__

class Ntp {
public:
  void onWifiConnected(time_t (*getNtpTime)(void));
  // Will convert current UTC to local time
  time_t getLocalTime();
  // Will syncronize with NTP server
  time_t getNtpTime();
  timeStatus_t getTimeStatus() { return timeStatus(); }

private:
  void sendNTPpacket(IPAddress &address);

  IPAddress _time_server;
  WiFiUDP _udp;
  static const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
  byte _buffer[NTP_PACKET_SIZE];         // buffer to hold incoming & outgoing packets
};

#endif //__NTP_H__
