#include "remote.h"
#include <FS.h>

#define FORCE_PUBLISH_INTERVAL_MS 30000

const char *HOME_ASSISTANT_CONFIG PROGMEM = "{ \
          \"name\": \"Clock\", \
          \"state_topic\": \"livingroom_clock/light/clock/state\", \
          \"command_topic\": \"livingroom_clock/light/clock/command\",  \
          \"brightness_state_topic\": \"livingroom_clock/light/clock/brightness/state\", \
          \"brightness_command_topic\": \"livingroom_clock/light/clock/brightness/command\",  \
          \"availability_topic\": \"livingroom_clock/status\", \
          \"unique_id\": \"ESP_clock_livingroom_clock\", \
          \"payload_on\":\"ON\", \
          \"payload_off\":\"OFF\", \
          \"device\": { \
          \"identifiers\": \"ee8996\", \
          \"name\": \"livingroom_clock\", \
          \"sw_version\": \"home brew 1.0\", \
          \"model\": \"PLATFORMIO_D1_MINI\", \
          \"manufacturer\": \"espressif\" \
          } \
          }";

Remote::Remote(Modekeeper &modekeeper, Rainbow &rainbow, String host, String username, String password)
    : _telnet_server(23), _web_server(80), _mqtt(_wifi_client), _modekeeper(modekeeper), _rainbow(rainbow), _host(host),
      _username(username), _password(password) {}

void Remote::setup() {
  _telnet_server.begin();
  _ws_server.listen(8080);

  SPIFFS.begin();

  _web_server.onNotFound([this]() {
    if (!Remote::handleFileRead(_web_server.uri())) {
      _web_server.send(404, "text/plain", "404: Not Found");
    }
  });

  _web_server.begin();

  _mqtt.setServer(_host.c_str(), 1883);
  _mqtt.setCallback([this](char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();

    // Naive parsing.
    String strtopic = String((char *)topic);
    if (strtopic.indexOf("brightness") > -1) {
      char buff[8];
      strncpy(buff, (char *)payload, min(length, (unsigned int)sizeof(buff)));
      int brightness = atoi(buff);
      Serial.println(brightness);
      _rainbow.setBrightness(brightness);
    } else {
      _modekeeper.setMode(strncmp((char *)payload, "ON", length) == 0 ? Modekeeper::Mode::RAINBOW
                                                                      : Modekeeper::Mode::CLOCK);
    }
  });
}

bool Remote::mqttMaybeReconnect() {
  if (_mqtt.connected()) {
    return true; // EARLY RETURN
  }

  if (_retry_to_connect_at_ms == 0 || millis() > _retry_to_connect_at_ms) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (_mqtt.connect("ESP8266livingroom_clock", _username.c_str(), _password.c_str(), "livingroom_clock/status", 0,
                      true, "offline")) {
      Serial.println("connected");
      bool published = _mqtt.publish_P("homeassistant/light/livingroom_clock/clock/config",
                                       (const uint8_t *)HOME_ASSISTANT_CONFIG, strlen(HOME_ASSISTANT_CONFIG), true);
      Serial.println("Published HA: " + String(published));
      _mqtt.subscribe("livingroom_clock/light/clock/command");
      _mqtt.subscribe("livingroom_clock/light/clock/brightness/command");
      const char *online = "online";
      _mqtt.publish("livingroom_clock/status", (const uint8_t *)online, strlen(online), true);
      _mqtt.publish("livingroom_clock/light/clock/state", "OFF");
      _mqtt.publish("livingroom_clock/light/clock/brightness/state", "0");
    } else {
      Serial.print("failed, rc=");
      Serial.print(_mqtt.state());
      Serial.println(" try again in 5 seconds");
      _retry_to_connect_at_ms = millis() + 5000;
    }
  }

  return _mqtt.connected();
}

bool Remote::handleFileRead(String path) {
  if (path.endsWith("/")) {
    path += "index.htm";
  }
  String contentType = "text/html";
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    size_t sent = _web_server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void Remote::handleData(int8_t c) {
  _modekeeper.stamp();
  switch (c) {
  case '1':
    _modekeeper.setMode(Modekeeper::Mode::SNAKE);
    break;
  case '2':
    _modekeeper.setMode(Modekeeper::Mode::CLOCK);
    break;
  case '3':
    _modekeeper.setMode(Modekeeper::Mode::RAINBOW);
    break;
  case '4':
    _modekeeper.setMode(Modekeeper::Mode::SINGLECOLOR);
    break;
  case '5':
    _modekeeper.setMode(Modekeeper::Mode::BRIGHTNESS);
    break;
  case 'a':
    _modekeeper.emitEvent(Modekeeper::Event::LEFT);
    break;
  case 'd':
    _modekeeper.emitEvent(Modekeeper::Event::RIGHT);
    break;
  }
}

void Remote::onMessage(WebsocketsClient &client, WebsocketsMessage &message) {
  handleData(message.rawData()[0]);
  client.send("Echo: " + message.data());
}

void Remote::pollAllWsClients() {
  for (auto &client : _ws_clients) {
    client.poll();
  }
}

void Remote::handle() {
  // Normal TCP socket
  if (_telnet_client && _telnet_client.connected()) {
    if (_telnet_client.available() > 0) {
      int8_t c = _telnet_client.read();
      Serial.println(String(c));
      handleData(c);
    }
  } else {
    _telnet_client = _telnet_server.available();
    if (_telnet_client) {
      _telnet_client.setTimeout(5000); // default is 1000
    }
  }

  // Websocket
  if (_ws_server.poll()) {
    auto ws_client = _ws_server.accept();
    ws_client.onMessage([this](WebsocketsClient &client, WebsocketsMessage message) { onMessage(client, message); });
    _ws_clients.push_back(ws_client);
  }

  pollAllWsClients();

  _web_server.handleClient();

  if (!mqttMaybeReconnect()) {
    return; // EARLY RETURN
  }

  _mqtt.loop();

  unsigned long now = millis();
  bool force_publish = now - _last_publish_ms > FORCE_PUBLISH_INTERVAL_MS;

  uint8_t brightness = _rainbow.getBrightness();
  if (_last_known_brightness != brightness || force_publish) {
    String strval = String(brightness);
    _mqtt.publish("livingroom_clock/light/clock/brightness/state", strval.c_str());
    _last_known_brightness = brightness;
    _last_publish_ms = now;
  }

  Modekeeper::Mode mode = _modekeeper.getMode();
  if (_last_known_mode != mode || force_publish) {
    _mqtt.publish("livingroom_clock/light/clock/state", mode == Modekeeper::Mode::RAINBOW ? "ON" : "OFF");
    _last_known_mode = mode;
    _last_publish_ms = now;
  }
}
