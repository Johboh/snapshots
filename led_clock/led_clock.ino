#include "FastLED.h"
#include "brightness.h"
#include "brightness_mode.h"
#include "clock.h"
#include "ledutils.h"
#include "modekeeper.h"
#include "ntp.h"
#include "rainbow.h"
#include "remote.h"
#include "singlecolor.h"
#include "snake.h"
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ElegantOTA.h>

// Using libraries
// https://github.com/ayushsharma82/ElegantOTA
// https://github.com/FastLED/FastLED
// https://github.com/JChristensen/Timezone
// https://github.com/ivanseidel/LinkedList
// https://github.com/knolleary/pubsubclient
// https://github.com/gilmaimon/ArduinoWebsockets

// Update at http://192.168.1.xxx:81/update

// LED specific
#define DATA_PIN D3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define DEFAULT_BRIGHTNESS 255

// WIFI setup
char HOSTNAME[] = "esp-clock";
char _ssid[] = "YOUR_SSID";
char _pass[] = "YOUR_PASSWORD";

// MQTT setup
const char MQTT_HOST[] = "192.168.1.xxx";
const char MQTT_USERNAME[] = "mqtt_username";
const char MQTT_PASSWORD[] = "mqtt_password";

const uint8_t kMatrixWidth = 21;
const uint8_t kMatrixHeight = 5;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

CRGB _leds_current[NUM_LEDS];

Ntp _ntp;
time_t getNtpTime() { _ntp.getNtpTime(); }

Brightness _brightness;
Ledutils ledutils(kMatrixWidth, kMatrixHeight);
BrightnessMode _brightness_mode(NUM_LEDS, ledutils, _brightness);
Clock _clock(NUM_LEDS, _ntp, ledutils, _brightness);
Snake _snake(kMatrixWidth, kMatrixHeight, ledutils);
Rainbow _rainbow(NUM_LEDS);
Singlecolor _singlecolor(NUM_LEDS);
Modekeeper _modekeeper(Modekeeper::Mode::CLOCK);
Remote _remote(_modekeeper, _rainbow, MQTT_HOST, MQTT_USERNAME, MQTT_PASSWORD);

ESP8266WebServer _elegant_ota_server(81);

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");

  // WiFi
  WiFi.hostname(HOSTNAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // NTP
  _ntp.onWifiConnected(getNtpTime);

  // OTA
  ArduinoOTA.onStart([]() { Serial.println("OTA: Start"); });
  ArduinoOTA.onEnd([]() { Serial.println("\nOTA: End"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA: Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA: Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  // ElegantOTA.setID("led_clock"); Does not work?
  ElegantOTA.begin(&_elegant_ota_server);
  _elegant_ota_server.begin();

  // Leds
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(_leds_current, NUM_LEDS)
      .setCorrection(TypicalLEDStrip)
      .setDither(false);
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);

  _remote.setup();
}

void loop()
{
  _brightness.handle();

  auto event = _modekeeper.getEvent();

  if (event == Modekeeper::Event::RESET)
  {
    FastLED.setBrightness(DEFAULT_BRIGHTNESS);
  }

  switch (_modekeeper.getMode())
  {
  case Modekeeper::Mode::SNAKE:
    switch (event)
    {
    case Modekeeper::Event::LEFT:
      _snake.rotateLeft();
      break;
    case Modekeeper::Event::RIGHT:
      _snake.rotateRight();
      break;
    case Modekeeper::Event::RESET:
      _snake.reset();
      break;
    }
    _snake.handle(_leds_current);
    break;
  case Modekeeper::Mode::CLOCK:
    _clock.handle(_leds_current);
    break;
  case Modekeeper::Mode::RAINBOW:
    _rainbow.handle(_leds_current);
    break;
  case Modekeeper::Mode::BRIGHTNESS:
    switch (event)
    {
    case Modekeeper::Event::RIGHT:
      _brightness_mode.increaseBrightness();
      break;
    case Modekeeper::Event::LEFT:
      _brightness_mode.decreaseBrightness();
      break;
    case Modekeeper::Event::RESET:
      _brightness_mode.reset();
      break;
    }
    _brightness_mode.handle(_leds_current);
    break;
  case Modekeeper::Mode::SINGLECOLOR:
    _singlecolor.handle(_leds_current);
    break;
  }
  _remote.handle();

  FastLED.show();
  ArduinoOTA.handle();
  _elegant_ota_server.handleClient();
}
