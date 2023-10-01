#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

AsyncWebServer server(80);

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#define RXD2 16
#define TXD2 17

const char* ssid = "POWER_LINE. #CHECK# Vol.1"; // Your WiFi AP SSID
const char* password = ""; // Your WiFi Password

const unsigned int MAX_MESSAGE_LENGTH = 12;
char input = 'X';
String web = "";

void Read_input() {

  //Check to see if anything is available in the serial receive buffer
  while (Serial2.available() > 0)
  {
    //Create a place to hold the incoming message
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;

    //Read the next available byte in the serial receive buffer
    char inByte = Serial2.read();

    //Message coming in (check not terminating character) and guard for over message size
    if ( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1) )
    {
      //Add the incoming byte to our message
      message[message_pos] = inByte;
      message_pos++;
    }
    //Full message received...
    else
    {
      //Add null character to string
      message[message_pos] = '\0';

      //Print the message (or do other things)
      Serial.println(message);
      input = message[0];

      //Reset for the next message
      message_pos = 0;
    }
  }
}


void recvMsg(uint8_t *data, size_t len) {
  WebSerial.println("Received Data...");
  String d = "";
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
  WebSerial.println(d);
}



void testscrolltext(void) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("192.168.4.1/webserial"));

  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print(millis() % 100);
  display.print(" > ");
  display.print(input);
  display.display();
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("MASTER_READ");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();

  testscrolltext();
}
int count = 0;

void loop() {
  Read_input();
  if (input == 'A' || input == 'B' || input == 'C') {
    Serial.println(input);
    testscrolltext();
    WebSerial.print(millis() % 100);
    WebSerial.print("\t>>>\t");
    WebSerial.println(input);
    count = 0;
  } else {
    input = 'X';
    testscrolltext();
    WebSerial.print(millis() % 100);
    WebSerial.print("\t>>>\t");
    WebSerial.println(input);
    delay(1000);
  }
}
