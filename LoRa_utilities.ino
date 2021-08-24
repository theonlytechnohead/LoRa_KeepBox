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
