// Local variables
int packet_rssi = 0;
float packet_snr = 0;

void drawUI () {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 0, String(LoRa.temperature()) + "°C");
  display.drawString(128, 10, String(packet_rssi, DEC) + " dBm");
  display.drawString(128, 20, String(packet_snr, 1) + " dB");

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.display();
}

void updateFrequencyOffset (void* parameter) {
  setFrequencyCorrection();
}

void setRSSI (int new_rssi) {
  packet_rssi = new_rssi;
}

void setSNR (float new_snr) {
  packet_snr = new_snr;
}

String getFormattedMillis () {
  unsigned long current = millis();
  unsigned long seconds = current / 1000;
  unsigned short decimal = current % 1000;
  String seconds_s = String(seconds);
  while (seconds_s.length() < 5) { seconds_s = " " + seconds_s; }
  String decimal_f;
  if (decimal < 100) { decimal_f += "0"; }
  if (decimal < 10) { decimal_f += "0"; }
  decimal_f += String(decimal);
  String output = "[" + seconds_s + "." + String(decimal_f) + "]";
  return output;
}

// serial printing
void printMessage (const char* tag, String message) {
  printMessage(tag, message.c_str());
}
void printMessage (String tag, const char* message) {
  printMessage(tag.c_str(), message);
}
void printMessage (String tag, String message) {
  printMessage(tag.c_str(), message.c_str());
}
void printMessage (const char* tag, const char* message) {
  String tag_s = String(tag) + ":";
  while (tag_s.length() < 8) { tag_s += " "; }
  Serial.println(getFormattedMillis() + " " + tag_s + String(message));
}
