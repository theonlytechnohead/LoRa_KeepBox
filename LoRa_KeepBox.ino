#include <LoRa.h>
#include <SSD1306.h>
#include "soc/rtc_wdt.h" // FreeRTOS WDT control for ESP32
#include <ArduinoJson.h>
#include "Timer.h"

//OLED pins
#define OLED_ADDRESS 0x3C
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define OLED_WIDTH 128 // OLED display width, in pixels
#define OLED_HEIGHT 64 // OLED display height, in pixels
#define OLED_FONT ArialMT_Plain_10

// Hardware variables
SSD1306 display(OLED_ADDRESS, OLED_SDA, OLED_SCL);

// Global constants / variables
TaskHandle_t background_loop;

void (*state)(); // function pointer, to hold the current state (i.e. function to execute)

// Local-ish variables
Timer t;
unsigned long last_millis = 0;

void setup () {
  Serial.begin(115200);
  pinMode(0, INPUT); // PRG button
  pinMode(2, OUTPUT); // Blue LED
  vTaskDelay(100);
  Serial.println("Booting...");

  init_OLED();
  display.drawString(0, 0, "Booting...");
  display.display();
  vTaskDelay(100);

  display.clear();
  init_LoRa();
  vTaskDelay(100);

  display.clear();
  display.drawString(0, 0, "Hello world!");
  display.display();

//  xTaskCreatePinnedToCore(
//      backgroundLoop, // Function to implement the task
//      "background loop", // Name of the task
//      8192,  // Stack size in words, causes stack overflow if too low
//      NULL,  // Task input parameter
//      0,  // Priority of the task, 0 is lowest
//      &background_loop,  // Task handle
//      0); // Core where the task should run, code runs on core 1 by default

  Serial.println("Boot completed");

  //t.every(5000, updateFrequencyOffset, (void*) 0);

//  setFrequencyCorrection();
  LoRa.receive();

  state = idle;
}

void loop () {
  //t.update();
//  int packetSize = LoRa.parsePacket();
//  if (packetSize) {
//    parseData(packetSize);
//  }
  state();
}

void backgroundLoop (void* parameter) {
  for (;;) { // Infinite loop
    vTaskDelay(10); // ESP32 defaults to 100Hz tick rate, so 10ms delay allows for 1 tick in order to run background tasks
    
  }
}

void init_OLED () {
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  vTaskDelay(20);
  digitalWrite(OLED_RST, HIGH);

  vTaskDelay(100);
  display.init();
  display.flipScreenVertically();
  display.setFont(OLED_FONT);
  display.clear();
  display.display();
}

void testSendLoRa (void* context) {
  //sendJSON(doc);
}

DynamicJsonDocument createJSON (String text) {
  DynamicJsonDocument doc(2048);
  doc["millis"] = millis();
  doc["text"] = text;
  return doc;
}
