/*
 * Ben Eater 8 Bit Computer - with added commands
 */

#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

#define HLT 0b1000000000000000  // Halt clock
#define MI  0b0100000000000000  // Memory Address Register IN
#define RI  0b0010000000000000  // RAM Data IN
#define RO  0b0001000000000000  // RAM Data OUT
#define IO  0b0000100000000000  // Instruction Register OUT
#define II  0b0000010000000000  // Instruction Register IN
#define AI  0b0000001000000000  // A Register IN
#define AO  0b0000000100000000  // A Register OUT
#define EO  0b0000000010000000  // ALU OUT
#define SU  0b0000000001000000  // ALU Subtract
#define BI  0b0000000000100000  // B Register IN
#define OI  0b0000000000010000  // Output Register IN
#define CE  0b0000000000001000  // Program Counter Enable
#define CO  0b0000000000000100  // Program Counter OUT
#define J   0b0000000000000010  // Jump (Program Counter IN)
#define FI  0b0000000000000001  // Flag Register IN

#define FLAGS_Z0C0 0
#define FLAGS_Z0C1 1
#define FLAGS_Z1C0 2
#define FLAGS_Z1C1 3

#define JNC 0b1001  // Jump to Address on Carry Flag NOT Set
#define JNZ 0b1010  // Jump to Address on Zero Flag NOT Set
#define JC  0b1101  // Jump to Address on Carry Flag Set
#define JZ  0b1110  // Jump to Address on Zero Flag Set


uint16_t UCODE_TEMPLATE[16][8] = {
  { CO|MI,  RO|II|CE,  0,      0,           0,           0, 0, 0 },   // 0000 - NOP
  { CO|MI,  RO|II|CE,  IO|MI,  RO|AI,       0,           0, 0, 0 },   // 0001 - LDA
  { CO|MI,  RO|II|CE,  IO|AI,  0,           0,           0, 0, 0 },   // 0010 - LDI
  { CO|MI,  RO|II|CE,  IO|MI,  RO|BI,       EO|AI|FI,    0, 0, 0 },   // 0011 - ADD
  { CO|MI,  RO|II|CE,  IO|MI,  RO|BI,       EO|AI|SU|FI, 0, 0, 0 },   // 0100 - SUB
  { CO|MI,  RO|II|CE,  IO|MI,  AO|RI,       0,           0, 0, 0 },   // 0101 - STA
  { CO|MI,  RO|II|CE,  IO|BI,  EO|AI|FI,    0,           0, 0, 0 },   // 0110 - INC
  { CO|MI,  RO|II|CE,  IO|BI,  EO|AI|SU|FI, 0,           0, 0, 0 },   // 0111 - DEC
  { CO|MI,  RO|II|CE,  AO|OI,  0,           0,           0, 0, 0 },   // 1000 - OUT
  { CO|MI,  RO|II|CE,  IO|J,   0,           0,           0, 0, 0 },   // 1001 - JNC
  { CO|MI,  RO|II|CE,  IO|J,   0,           0,           0, 0, 0 },   // 1010 - JNZ
  { CO|MI,  RO|II|CE,  IO|MI,  RO|OI,       0,           0, 0, 0 },   // 1011 - DSP
  { CO|MI,  RO|II|CE,  IO|J,   0,           0,           0, 0, 0 },   // 1100 - JMP
  { CO|MI,  RO|II|CE,  0,      0,           0,           0, 0, 0 },   // 1101 - JC
  { CO|MI,  RO|II|CE,  0,      0,           0,           0, 0, 0 },   // 1110 - JZ
  { CO|MI,  RO|II|CE,  HLT,    0,           0,           0, 0, 0 },   // 1111 - HLT
};

uint16_t ucode[4][16][8];

void initUCode() {
  // ZF = 0, CF = 0
  memcpy(ucode[FLAGS_Z0C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));

  // ZF = 0, CF = 1
  memcpy(ucode[FLAGS_Z0C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_Z0C1][JC][2]  = IO|J;
  ucode[FLAGS_Z0C1][JNC][2] = 0;

  // ZF = 1, CF = 0
  memcpy(ucode[FLAGS_Z1C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_Z1C0][JZ][2]  = IO|J;
  ucode[FLAGS_Z1C0][JNZ][2] = 0;

  // ZF = 1, CF = 1
  memcpy(ucode[FLAGS_Z1C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_Z1C1][JC][2]  = IO|J;
  ucode[FLAGS_Z1C1][JZ][2]  = IO|J;
  ucode[FLAGS_Z1C1][JNC][2] = 0;
  ucode[FLAGS_Z1C1][JNZ][2] = 0;
}

/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}


/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}


/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}


/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void printContents(int start, int length) {
  for (int base = start; base < length; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


void setup() {
  // put your setup code here, to run once:
  initUCode();

  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);

  // Program data bytes
  Serial.print("Programming EEPROM");

  // Program the 8 high-order bits of microcode into the first 128 bytes of EEPROM
  for (int address = 0; address < 1024; address += 1) {
    int byte_sel    = (address & 0b1000000000) >> 9;
    int flags       = (address & 0b0110000000) >> 7;
    int instruction = (address & 0b0001111000) >> 3;
    int step        = (address & 0b0000000111);

    if (byte_sel) {
      writeEEPROM(address, ucode[flags][instruction][step]);
    } else {
      writeEEPROM(address, ucode[flags][instruction][step] >> 8);
    }

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }

  Serial.println(" done");


  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents(0, 1024);
}


void loop() {
  // put your main code here, to run repeatedly:

}
