//LoRa pins
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26
#define LORA_FREQ 915E6
#define LORA_SYNCWORD 232 //HEX: 0xE8 // ranges from 0x00-0xFF, default 0x12, see API docs

void init_LoRa () {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  
  if (LoRa.begin(LORA_FREQ)) {
    display.clear();
    display.drawString(0, 0, "LoRa setup complete!");
    display.display();
    vTaskDelay(100);
  } else {
    display.clear();
    display.drawString(0, 0, "LoRa borked");
    display.display();
    while (1) {
      rtc_wdt_feed(); // satiate the WDT
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
  
  // Magic LoRa settings
  LoRa.sleep();
  LoRa.setSyncWord(LORA_SYNCWORD);
  LoRa.setTxPower(20); // (dB) max legally allowed and using PA_BOOST pin
  LoRa.setSpreadingFactor(10);
  LoRa.setCodingRate4(6);
  LoRa.setFrequency(LORA_FREQ);
  LoRa.setSignalBandwidth(125E3); // this SF, CR, and BW make for 433 bps (no, not even Kbps)
  LoRa.enableCrc();
  LoRa.idle();
}

void sendJSON (DynamicJsonDocument doc) {
  // Prepare for sending and turn on LED
  LoRa.idle();
  digitalWrite(2, HIGH);

  char output[measureJson(doc) + 1];
  serializeJson(doc, output, sizeof(output));

  // send packet
  LoRa.beginPacket();
  LoRa.print(output);
  LoRa.endPacket(); // blocks until packet is done
  // Here it is safe to receive again
  LoRa.receive();
  // LED off
  digitalWrite(2, LOW);
}

void parseData (int packetSize) {
  digitalWrite(2, HIGH);
  String packet;
  for (int i = 0; i < packetSize; i++) {
    packet += (char) LoRa.read();
  }
  Serial.println(packet);
  setRSSI(LoRa.packetRssi());
  setSNR(LoRa.packetSnr());
  //rssi = String(LoRa.packetRssi(), DEC);
  //snr = String(LoRa.packetSnr(), 1);

  DynamicJsonDocument doc(2048);

  deserializeJson(doc, packet);

  drawUI();
  display.drawString(0, 0, doc["text"]);
  display.drawString(0, 10, doc["millis"]);
  display.display();
  digitalWrite(2, LOW);
}
