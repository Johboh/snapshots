#include "Ntp.h"
#include <Timezone.h>

namespace {
// Swedish specific NTP server and timezone
const char *ntp_server_name = "ntp1.sptime.se";
const TimeChangeRule eu_CEST = {"CEST", Last, Sun, Mar, 1, 120}; // UTC + 2
                                                                 // hours
const TimeChangeRule eu_CET = {"CET", Last, Sun, Oct, 1, 60};    // UTC + 1 hours
Timezone eu_central(eu_CEST, eu_CET);
const unsigned int local_udp_port = 8888;
const unsigned int sync_interval_s = 180;
} // namespace

void Ntp::onWifiConnected(time_t (*getNtpTime)(void)) {
  Serial.println("Ntp::onWifiConnected()");
  WiFi.hostByName(ntp_server_name, _time_server);
  _udp.begin(local_udp_port);
  setSyncProvider(getNtpTime);
  setSyncInterval(sync_interval_s);
}

time_t Ntp::getLocalTime() { return eu_central.toLocal(now()); }

time_t Ntp::getNtpTime() {
  Serial.print("Ntp::getNtpTime(): ");
  while (_udp.parsePacket() > 0)
    ; // discard any previously received packets
  sendNTPpacket(_time_server);
  uint32_t begin_wait = millis();
  while (millis() - begin_wait < 1500) {
    int size = _udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      _udp.read(_buffer, NTP_PACKET_SIZE); // read packet into the buffer
      unsigned long secs_since_1900;
      // convert four bytes starting at location 40 to a long integer
      secs_since_1900 = (unsigned long)_buffer[40] << 24;
      secs_since_1900 |= (unsigned long)_buffer[41] << 16;
      secs_since_1900 |= (unsigned long)_buffer[42] << 8;
      secs_since_1900 |= (unsigned long)_buffer[43];
      time_t t = secs_since_1900 - 2208988800UL;
      Serial.println(t);
      return t;
    }
  }
  Serial.println("Timeout.");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void Ntp::sendNTPpacket(IPAddress &address) {
  // set all bytes in the buffer to 0
  memset(_buffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  _buffer[0] = 0b11100011; // LI, Version, Mode
  _buffer[1] = 0;          // Stratum, or type of clock
  _buffer[2] = 6;          // Polling Interval
  _buffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  _buffer[12] = 49;
  _buffer[13] = 0x4E;
  _buffer[14] = 49;
  _buffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  _udp.beginPacket(address, 123); // NTP requests are to port 123
  _udp.write(_buffer, NTP_PACKET_SIZE);
  _udp.endPacket();
}
