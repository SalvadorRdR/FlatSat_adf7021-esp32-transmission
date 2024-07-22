#include <SPI.h>

// Data Pins
#define RECIEVE_PIN 1
#define TRANSMIT_PIN 3
#define CLK_OUT_PIN 21

// Setup Pins
#define CE_PIN 5
#define SREAD_PIN 19
#define SDATA_PIN 23
#define SCLK_PIN 18
#define SLE_PIN 32


const uint32_t REG0 = 0x11616A20;
const uint32_t REG1 = 0x00475011;
const uint32_t REG2 = 0x003DD082;  
const uint32_t REG3 = 0x2B149923; 
const uint32_t REG15 = 0x000E000F;

void setupPins(){
  pinMode(SLE_PIN, OUTPUT);
  pinMode(TRANSMIT_PIN, OUTPUT);
  pinMode(RECIEVE_PIN, INPUT);
  pinMode(CLK_OUT_PIN, INPUT);
  pinMode(SCLK_PIN, OUTPUT);

  digitalWrite(SLE_PIN, LOW); 
  digitalWrite(TRANSMIT_PIN, LOW); 
}

void writeRegister(uint32_t data, uint8_t length) {
  // Calculate number of bytes to send
  int numBytes = (length + 7) / 8; 
  
  // Send each byte over SPI
  for (int i = numBytes - 1; i >= 0; i--) {
    SPI.transfer((uint8_t)(data >> (i * 8))); // Send each byte, MSB first
  }
}

void loadRegister(){
  digitalWrite(SLE_PIN, HIGH); 
  delay(1);
  digitalWrite(SLE_PIN, LOW); 
}

void setupTransmitter(){
  // Configure SPI for setup
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  digitalWrite(CE_PIN, HIGH); // Enable chip
  delay(1); // Wait for XTAL

  // Set registers
  writeRegister(REG1, 26);
  loadRegister();
  delay(1);

  writeRegister(REG3, 32);
  loadRegister();
  
  writeRegister(REG0, 32);
  loadRegister();
  delay(1);

  writeRegister(REG2, 31);
  loadRegister();
  
  writeRegister(REG15, 32);
  loadRegister();
  
  Serial.println("Transmission setup");
}

void transmitData(uint32_t data) {
  // Ensure that the data is clocked in on the positive edge of CLKOUT
  for (int i = 31; i >= 0; i--) {
    // Wait for the positive edge of CLKOUT
    while (digitalRead(CLK_OUT_PIN) == LOW);
    while (digitalRead(CLK_OUT_PIN) == HIGH);

    digitalWrite(TRANSMIT_PIN, (data >> i) & 0x01);
  }
  Serial.println("Data sent");
}

void setup() {
  setupPins();
  Serial.begin(115200);
  setupTransmitter();  
}

void loop() {
  // transmit dummy data
  transmitData(0xAA);
  delay(100);
}