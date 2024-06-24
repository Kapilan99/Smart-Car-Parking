#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE, CSN pins
const byte address[6] = "00001"; 

#define ir_car1 4
#define ir_car2 5
#define ir_car3 6
#define ir_car4 7

int slotStatus[4];

void setup() {
  pinMode(ir_car1, INPUT);
  pinMode(ir_car2, INPUT);
  pinMode(ir_car3, INPUT);
  pinMode(ir_car4, INPUT);

  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  slotStatus[0] = digitalRead(ir_car1) == 0 ? 1 : 0;
  slotStatus[1] = digitalRead(ir_car2) == 0 ? 1 : 0;
  slotStatus[2] = digitalRead(ir_car3) == 0 ? 1 : 0;
  slotStatus[3] = digitalRead(ir_car4) == 0 ? 1 : 0;

  if (radio.available()) {
    radio.stopListening();
    radio.write(&slotStatus, sizeof(slotStatus));
    radio.startListening();
  }
}
