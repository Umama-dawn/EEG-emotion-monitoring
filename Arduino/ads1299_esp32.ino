// ADS1299 basic continuous read (single ADS1299, adapt for 2x ADS if needed)
#include <SPI.h>

#define DRDY  4
#define CS    5
#define RESET 2

void ads_write(uint8_t address, uint8_t data) {
  digitalWrite(CS, LOW);
  SPI.transfer(0x40 | address);
  SPI.transfer(0x00);
  SPI.transfer(data);
  digitalWrite(CS, HIGH);
}

void ads_command(uint8_t cmd) {
  digitalWrite(CS, LOW);
  SPI.transfer(cmd);
  digitalWrite(CS, HIGH);
}

void setup() {
  Serial.begin(115200);
  SPI.begin(18, 19, 23, CS); // SCK, MISO, MOSI, SS

  pinMode(CS, OUTPUT);
  pinMode(DRDY, INPUT_PULLUP);
  pinMode(RESET, OUTPUT);

  digitalWrite(RESET, LOW);
  delay(50);
  digitalWrite(RESET, HIGH);
  delay(100);

  ads_command(0x06); // RESET
  delay(100);

  ads_command(0x11); // SDATAC (stop continuous)
  delay(10);

  ads_write(0x01, 0x96); // CONFIG1: 250 SPS
  ads_write(0x02, 0xD0); // CONFIG2
  ads_write(0x03, 0xE0); // CONFIG3

  // Set CHnSET registers (enable channels, gain=24)
  for (uint8_t i = 0; i < 8; i++) {
    ads_write(0x05 + i, 0x60); // 0x60 => Power on, gain 24, input normal
  }

  ads_command(0x10); // START
  ads_command(0x08); // RDATAC (start continuous read)
}

long conv24ToLong(uint8_t b1, uint8_t b2, uint8_t b3) {
  long val = ((long)b1 << 16) | ((long)b2 << 8) | b3;
  if (val & 0x800000) val |= 0xFF000000; // sign extend
  return val;
}

void loop() {
  if (digitalRead(DRDY) == LOW) {
    digitalWrite(CS, LOW);
    // read status(3) + 8 channels * 3 bytes = 27 bytes
    uint8_t buf[27];
    for (int i=0;i<27;i++) buf[i] = SPI.transfer(0x00);
    digitalWrite(CS, HIGH);

    // convert to microvolts (approx). Adjust Vref/gain according to your board.
    const float Vref = 4.5;
    const float gain = 24.0;
    const float LSB_uV = (Vref / (pow(2,23)-1)) / gain * 1e6;

    // status is buf[0..2], channels start at buf[3]
    long chRaw[8];
    int p = 3;
    for (int ch=0; ch<8; ch++) {
      chRaw[ch] = conv24ToLong(buf[p], buf[p+1], buf[p+2]);
      p += 3;
    }

    unsigned long ts = millis();
    // Output CSV line: ts,ch1_uV,...ch8_uV
    Serial.print(ts);
    for (int ch=0; ch<8; ch++) {
      float uV = chRaw[ch] * LSB_uV;
      Serial.print(",");
      Serial.print(uV, 6);
    }
    Serial.println();
  }
}
