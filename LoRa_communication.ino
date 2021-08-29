// implement FSM!
// link/source: https://www.youtube.com/watch?v=pxaIyNbcPrA

#include <CircularBuffer.h>
CircularBuffer<StaticJsonDocument<256>,4> send_queue;
void (*prev)(); // stores the previous state, to aid in state transitions
int packetSize = 0;

void idle () {
  if (prev != state) {
    printMessage("lora", "state=idle");
//    digitalWrite(2, LOW); // LED off
    LoRa.receive();
    prev = state;
  }
  
  if (!send_queue.isEmpty()) {
    prev = state;
    state = send;
    return;
  } else {
    packetSize = LoRa.parsePacket();
    if (packetSize) {
      prev = state;
      state = receive;
      return;
    }
  }
}

void send () {
  printMessage("lora", "state=send");

  StaticJsonDocument<256> doc = send_queue.first(); // pull from the front
  
  char output[measureJson(doc) + 1];
  serializeJson(doc, output, sizeof(output));
  // TEMP?: Display packet
//  drawPacket(doc);

  digitalWrite(2, HIGH);
  LoRa.beginPacket();
  LoRa.print(output);
  LoRa.endPacket();
  digitalWrite(2, LOW);
  
  prev = state;
  state = wait;
  return;
}

unsigned long startWaitTime;
unsigned short retries = 0; // for retransmit
void wait () {
  if (prev != state) {
    printMessage("lora", "state=wait");
    startWaitTime = millis();
    LoRa.receive();
    prev = state;
  }

  // try receiving
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String packet;
    for (int i = 0; i < packetSize; i++) {
      packet += (char) LoRa.read();
    }
    if (packet == "ACK") {
      send_queue.shift(); // dequeue (pop)
      retries = 0;
      prev = state;
      state = idle;
      return;
    } else { // we didn't get a proper ACK, try again
      prev = state;
      state = retransmit;
      return;
    }
  }

  if (millis() - startWaitTime >= 1000) { // check timeout
    prev = state;
    state = retransmit;
    return;
  }
}

void retransmit () {
  printMessage("lora", "state=retransmit, retries=" + String(retries));
  // check retries
  if (retries < 4) {
    retries++;
    // retransmit
    prev = state;
    state = send;
    return;
  } else {
    printMessage("lora", "dropped packet after max retries");
    retries = 0;
    prev = state;
    state = idle;
    return;
  }
}

void receive () {
  printMessage("lora", "state=receive");
  String packet;
  for (int i = 0; i < packetSize; i++) {
    packet += (char) LoRa.read();
  }
  
  // TODO: verify checksum, nak on fail

  printMessage("lora", "received message, " + packet);

  StaticJsonDocument<256> doc;
  setRSSI(LoRa.packetRssi());
  setSNR(LoRa.packetSnr());

  deserializeJson(doc, packet);

  drawUI();
  display.drawString(0, 0, doc["count"]);
  display.drawString(0, 10, doc["text"]);
  display.drawString(0, 20, doc["millis"]);
  display.display();
  
  prev = state;
  state = ack;
  return;
}

void ack () {
  printMessage("lora", "state=ack");
  digitalWrite(2, HIGH);
  LoRa.beginPacket();
  LoRa.print("ACK");
  LoRa.endPacket();
  digitalWrite(2, LOW);

  prev = state;
  state = idle;
  return;
}

void nak () {
  printMessage("lora", "stat=nak");
  digitalWrite(2, HIGH);
  LoRa.beginPacket();
  LoRa.print("NAK");
  LoRa.endPacket();
  digitalWrite(2, LOW);

  prev = state;
  state = idle;
  return;
}
