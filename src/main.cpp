#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>


// LEDs and pins
#define PIN_D_RGB_G 12
#define PIN_D_RGB_B 13
#define PIN_D_RGB_R 15

#define LEDON    LOW
#define LEDOFF   HIGH

// Bot specific
#define LED_COUNT 10
#define LED_PIN 14

#define LED_UPDATE_SPEED 20
#define LED_PRIDE_SPEED 500

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


// global vars
bool              PIN_RGB_ON = 0;
int               wheelTicker = 0;

// objects
Ticker           ticker;
WiFiClient       espClient;
void mqttCallback(char* topic, byte* payload, unsigned int length);
PubSubClient mqttClient(espClient);



///////////////////////////////////////////////////////////////////////////
//   LED Ticker
///////////////////////////////////////////////////////////////////////////
void tickBlue() {
  PIN_RGB_ON = !PIN_RGB_ON;
  digitalWrite(PIN_D_RGB_B, PIN_RGB_ON);
}
void tickerOff() {
  ticker.detach();
  digitalWrite(PIN_D_RGB_B, LOW);
}



///////////////////////////////////////////////////////////////////////////
//   LEDs
///////////////////////////////////////////////////////////////////////////
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint8_t splitColor ( uint32_t c, char value ) {
  switch ( value ) {
    case 'r': return (uint8_t)(c >> 16);
    case 'g': return (uint8_t)(c >>  8);
    case 'b': return (uint8_t)(c >>  0);
    default:  return 0;
  }
}

void ledLoop () {
  if(wheelTicker > (255 + LED_PRIDE_SPEED) || wheelTicker < 0) {
    wheelTicker = 0;
  }

  for(int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + wheelTicker) & 255));
  }

  wheelTicker++;
  strip.show();
}




void setup()
{
  Serial.begin(9600);
  ticker.attach(0.1, tickBlue);

  Serial.print("ESP ");
  Serial.println(ESP.getChipId());
  Serial.println("--------------------");

  pinMode(PIN_D_RGB_R, OUTPUT);
  pinMode(PIN_D_RGB_G, OUTPUT);
  pinMode(PIN_D_RGB_B, OUTPUT);

  Serial.println("- Setup LED stripe");
  strip.begin();
  strip.setBrightness(255);

  Serial.println("- Setup completed\n");
  tickerOff();
}

void loop()
{
  ledLoop();
  yield();

  delay(LED_UPDATE_SPEED);
}
