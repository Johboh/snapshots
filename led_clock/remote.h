#include "PubSubClient.h"
#include "modekeeper.h"
#include "rainbow.h"
#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <vector>

#ifndef __REMOTE_H__
#define __REMOTE_H__

using namespace websockets;

class Remote {
public:
  Remote(Modekeeper &modekeeper, Rainbow &rainbow, String host, String username, String password);
  void setup();
  void handle();

private:
  void handleData(int8_t c);
  void onMessage(WebsocketsClient &client, WebsocketsMessage &message);
  void pollAllWsClients();
  bool handleFileRead(String path);
  bool mqttMaybeReconnect();

private:
  WiFiServer _telnet_server;
  WiFiClient _telnet_client;

  WebsocketsServer _ws_server;
  std::vector<WebsocketsClient> _ws_clients;

  ESP8266WebServer _web_server;

  WiFiClient _wifi_client;
  PubSubClient _mqtt;
  unsigned long _retry_to_connect_at_ms = 0;

  Modekeeper &_modekeeper;
  Rainbow &_rainbow;

  Modekeeper::Mode _last_known_mode;
  uint8_t _last_known_brightness;
  unsigned long _last_publish_ms;

  String _host;
  String _username;
  String _password;
};

#endif //__REMOTE_H__
