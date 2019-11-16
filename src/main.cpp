/*
  Based on ESP8266 mDNS responder sample code
*/

#include <Arduino.h>

#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#include <FastLED.h>

#define FASTLED_USING_NAMESPACE

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <utils/HTTP.h>

#define LED_CONTROL_PIN 2  
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define DEF_BRIGHTNESS 90
#define DEF_COLOR CRGB::White 

#define FRAMES_PER_SECOND 120

#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);
HTTPClient http;

int LIGHT_MODE = 0;

void default_behavior(bool f);
void full_brightness(bool f);
void full_red(bool f);
void off(bool f);
void uwu(bool f);
void on(bool f);
void sinelon(bool f);
// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])(bool first);
SimplePatternList LIGHT_PROCEDURES = { default_behavior, full_brightness, full_red, off, uwu, on, sinelon};


void setup(void) {
  
  pinMode(2, OUTPUT);

  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("blinkyboy")) {
    Serial.println("Error setting up MDNS responder!");
    delay(1000);
    ESP.restart();
  }
  Serial.println("mDNS responder started");

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);

  //Setup leds
  FastLED.addLeds<LED_TYPE, LED_CONTROL_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(LEDColorCorrection::TypicalSMD5050);

  FastLED.clear(true);

  digitalWrite(2,HIGH);
}

//Databuffer for incoming requests
byte eorfsm;

bool req_fsm(char c){
  switch(eorfsm){
    default:
      eorfsm = 0;
    case 0:
      if(c == '\r'){ eorfsm = 1; break; }
      break;
    case 1:
      if(c == '\n'){ eorfsm = 2; break; }
      eorfsm = 0; break;
    case 2:
      if(c == '\r'){ eorfsm = 3; break; }
      eorfsm = 0; break;
    case 3:
      if(c == '\n'){ eorfsm = 0; return true; }
      eorfsm = 0; break;
  }
  return false;
}

String dbuf;
bool eor = false; 

bool process_client(WiFiClient client){
  int size = 0;
  if(!eor && (size = client.available())==0){
    return false;
  }else if(!eor){
    digitalWrite(2, LOW);
    for(int i = 0; i < size; i++){
      char c = client.read();
      eor = req_fsm(c);
      dbuf += c;
    }
    return false;
  }
  digitalWrite(2, HIGH);
  client.flush();

  HTTPMethod m = parse_option(dbuf.c_str());

  Serial.printf("Input: %s\nParsed method: %d %s\n", dbuf.c_str(), m, option_str(m));

  // Read the first line of HTTP request
  int le = dbuf.indexOf('\r');
  String req = dbuf.substring(0,le);

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return true;
  }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print("Request: ");
  Serial.println(req);

  String s;
  if (req == "/hewwo") {
    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
    s += ipStr;
    s += "</html>\r\n\r\n";
    Serial.println("Sending 200");
  } else if(req == "/full") {
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTurning on!\r\n\r\n";
    LIGHT_MODE = 1;
  } else if(req == "/on") {
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTurning on!\r\n\r\n";
    LIGHT_MODE = 2;
  } else if(req == "/off") {
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTurning off!\r\n\r\n";
    LIGHT_MODE = 3;
  } else if(req == "/uwu") {
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nUh oh, better close your eyes!\r\n\r\n";
    LIGHT_MODE = 4;
  } else if(req == "/sine") {
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nUh oh, better close your eyes!\r\n\r\n";
    LIGHT_MODE = 6;
  } else if(req.startsWith("/color?0x")) {
    String cs = req.substring(9,15);
    int col = strtol(cs.c_str(),0,16);
    Serial.printf("Setting color to %s = %d\n", cs.c_str(), col);
    fill_solid(leds, NUM_LEDS, col);
    LIGHT_MODE = 5;
  } else {
    s = "HTTP/1.1 404 Not Found\r\n\r\n";
    Serial.println("Sending 404");
  }
  client.print(s);

  Serial.println("Done with client");
  eor = false;
  dbuf = "";
  return true;
}


unsigned long start = 0;
byte fsm = 0;
WiFiClient client;


void loop(void) {
  MDNS.update();

  int mode = LIGHT_MODE;
  // Check if a client has connected
  switch(fsm){
    case 0:
      if(client.connected()||!(client = server.available())){
        break;
      }
      Serial.println("");
      Serial.println("New client");
      fsm = 1;
      start = millis();
    case 1:
      // Wait for data from client to become available
      if (client.connected() && process_client(client)) {
        client.stop();
         fsm = 0;
      }else if((start + 10000u) < millis()){
        Serial.println("Client Timeout");
        client.flush();
        client.stop();
        fsm = 0;
      }
  }

  LIGHT_PROCEDURES[LIGHT_MODE](mode != LIGHT_MODE);

  FastLED.delay(1000/FRAMES_PER_SECOND);
}

void default_behavior(bool first) {
  if(first){
    FastLED.setBrightness(DEF_BRIGHTNESS);
    #ifdef DEF_COLOR
    fill_solid( leds, NUM_LEDS, DEF_COLOR);
    #endif
  }
}

void full_brightness(bool first) {
  if(first){
    fill_solid( leds, NUM_LEDS, CRGB::White);
    FastLED.setBrightness(255);
  } 
}

void full_red(bool first) {
  if(first){
    fill_solid( leds, NUM_LEDS, CRGB::Red);
    FastLED.setBrightness(255); 
  }
}

void on(bool first) {
  if(first){
    FastLED.setBrightness(255); 
  }
}

void off(bool first) {
  if(first){
    FastLED.setBrightness(0);
  }
}

void uwu(bool first) {
  if(first){
    FastLED.setBrightness(255);
  }
  fill_rainbow( leds, NUM_LEDS, millis()/10, 7);
}

int count = 0;
void sinelon(bool first) {
  if(first){
    FastLED.setBrightness(255);
  }
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 50);
  leds[count++] += CHSV( millis()/10, 255, 192);
  count%=NUM_LEDS;
}
