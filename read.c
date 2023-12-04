/* Copyright (C) 2023 Nicholas Carlini.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include <util/delay.h>

#define PCMD 10
#define PSTRB 11
#define DRW 12
#define CRES 13
#define BSY 14
#define PARITY 15


void do_setup() {
  pinMode(PSTRB, OUTPUT);
  pinMode(DRW, OUTPUT);
  pinMode(CRES, OUTPUT);
  pinMode(PCMD, OUTPUT);
  pinMode(BSY, INPUT);
  digitalWrite(PSTRB, 1);
  digitalWrite(DRW, 1);
  digitalWrite(CRES, 1);
  digitalWrite(PCMD, 1);
}


void setup_read_bus() {
  digitalWrite(DRW, 1);
  // set other pins to read mode
  for (int i = 0; i < 8; i++)
    pinMode(i+2, INPUT);
  pinMode(PARITY, INPUT);
}

int read_byte() {
  int o = 0;
  digitalWrite(PSTRB, 0);
  for (int i = 0; i < 8; i++)  {
    o |= digitalRead(i+2)<<i;
  }
  int parity = digitalRead(PARITY);
  delayMicroseconds(5);
  digitalWrite(PSTRB, 1);
  delayMicroseconds(5);
  return (parity << 8) + o;
}

void setup_write_bus() {
  digitalWrite(DRW, 0);
  // set other pins to write mode
  for (int i = 0; i < 8; i++) 
    pinMode(i+2, OUTPUT);
  pinMode(PARITY, OUTPUT);
}

void write_bus(int byte) {
  int xorb = 1;
  for (int i = 0; i < 8; i++) {
    digitalWrite(i+2, (byte>>i)&1);
    xorb ^= (byte>>i)&1;
  }
  digitalWrite(PARITY, xorb);
}

void write_cmd(int byte) {
  digitalWrite(PSTRB, 1);
  delayMicroseconds(100);
  write_bus(byte);
  delayMicroseconds(100);
  digitalWrite(PSTRB, 0);
  delay(1);
}


char byt[2*(256+256+4+20)];

void read(int sector1, int sector2) {

    Serial.print("Block: ");
    Serial.print(sector1);
    Serial.print(" ");
    Serial.println(sector2);

  while (!digitalRead(BSY)); // wait for BSY to go high for setup

  delay(2);
  do_setup();
  delay(2);

  // step 1: lower pcmd
  digitalWrite(PCMD, 0);
  setup_read_bus();
  delay(2);

  // step 2: wait for BSY to go low
  while (digitalRead(BSY)); // wait for BSY to go low
  delay(2);

  // step 3: read the 0x01 on the bus
  int is_ok = read_byte();
  Serial.println(is_ok);
  if (is_ok != 0x1) {
      Serial.println("ERROR: Didn't read 1\n");
      while (1) {

        int x = read_byte();
        Serial.println(x);
        delay(500);
      }
      return;
  }

  // step 4: write a 0x55 on the bus
  setup_write_bus();
  write_bus(0x55);
  delay(2);

  // step 4.5: tell the profile we wrote it
  digitalWrite(PCMD, 1);
  delay(2);

  // step 5: wait for the drive to say it's read it
  while (!digitalRead(BSY)); // wait for BSY to go high

  delay(2);

  // step 6: write command bytes
  write_cmd(0x00); // read
  write_cmd(0x00); // location 00
  write_cmd(sector1); // location high
  write_cmd(sector2); // location low
  write_cmd(0x64); // retry 100
  write_cmd(0x14); // no idea but set it to 0x14
  delay(1);

  // step 7: done write command bytes
  digitalWrite(PSTRB, 1);

  delay(1);
  setup_read_bus();
  delay(1);
  digitalWrite(PCMD, 0);

  // step 8: wait for read
  while (digitalRead(BSY)); // wait for BSY to go low

  int read_ack = read_byte();
  if (read_ack != 0x2) {
      Serial.println("ERROR: Didn't read 2\n");
      while (1) {

        int x = read_byte();
        Serial.println(x);
        delay(500);
      }
      return;
  }

  // step 9: write a 0x55 on the bus
  setup_write_bus();
  write_bus(0x55);

  // step 9.5: tell the profile we wrote it
  digitalWrite(PCMD, 1);

  // step 10: data has been read
  while (!digitalRead(BSY)); // wait for BSY to go high

  setup_read_bus();


  for (int i = 0; i < 2*(4 + 256 + 256 + 20);) {
    int n = read_byte();
    byt[i++] = (n & 0xF) + 0x40;
    byt[i++] = (n >> 4) + 0x40;

  }

  Serial.print("<");
  Serial.write(byt, 2*(256+256+4+20));
  Serial.println(">");
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Done setup waiting to go");
  while (Serial.available() == 0);

  Serial.println("GO!");

  for (int i = 0; i < 0x2600; i++) {
    read((i>>8), i&0xFF);
    delay(10);
  }
  Serial.println("Done reading these");

}

void loop() {
  // nothing to do
}
