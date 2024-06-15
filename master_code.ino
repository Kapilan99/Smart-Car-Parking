#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Initialize LCD display, RF24 radio, and Servo objects
LiquidCrystal_I2C lcd(0x27, 16, 2);
RF24 radio(7, 8);  // CE, CSN pins
Servo entranceServo;
Servo exitServo;

// Define addresses for communication with the parking set controllers
const byte addresses[][6] = {"00001", "00002"};

// Define IR sensor pins
#define ir_enter1 2
#define ir_enter2 3
#define ir_exit1 4
#define ir_exit2 5

// Variables to hold the status of slots from two parking set controllers
int slotStatus[2][4]; 
int reservedSlotSet = -1; // Reserved slot parking set index
int reservedSlot = -1; // Reserved slot index within the set
unsigned long reservationTime; // Time when the slot was reserved
const unsigned long reservationTimeout = 30000; // 30 seconds timeout

void setup() {
  // Initialize LCD display
  lcd.init();
  lcd.backlight();

  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Set IR sensor pins as inputs
  pinMode(ir_enter1, INPUT);
  pinMode(ir_enter2, INPUT);
  pinMode(ir_exit1, INPUT);
  pinMode(ir_exit2, INPUT);

  // Attach servo motors to corresponding pins and set initial positions
  entranceServo.attach(10); // Entrance gate servo motor pin connected to D10
  exitServo.attach(11); // Exit gate servo motor pin connected to D11
  entranceServo.write(40);  // sets the servo at 40 degree position
  exitServo.write(40);  // sets the servo at 40 degree position

  // Initialize the RF24 radio and open reading pipes for communication
  radio.begin();
  for (int i = 0; i < 2; i++) {
    radio.openReadingPipe(i + 1, addresses[i]); // Addresses for the parking sets
  }
  radio.setPALevel(RF24_PA_LOW);

  // Display initial message on the LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(" Smart Car ");
  lcd.setCursor(0, 1);
  lcd.print(" Parking System ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Welcome ");
}

void loop() {
  // Check if the reservation has timed out
  checkReservationTimeout();
  
  // Request the slot status from parking set controllers
  requestSlotStatus();

  // Handle vehicle entry or exit based on IR sensor input
  if (digitalRead(ir_enter1) == 0 || digitalRead(ir_enter2) == 0) {
    handleVehicleEntry();
  } else if (digitalRead(ir_exit1) == 0 || digitalRead(ir_exit2) == 0) {
    handleVehicleExit();
  } else {
    // Display welcome message and close gates if no vehicle is detected
    displayWelcomeMessage();
    entranceServo.write(40); // Close the entrance gate
    exitServo.write(40); // Close the exit gate
  }
}

void requestSlotStatus() {
  // Start listening for data from parking set controllers
  radio.startListening();
  delay(5); // Short delay to ensure readiness
  for (int i = 0; i < 2; i++) {
    if (radio.available()) {
      // Read the slot status data from each parking set controller
      radio.read(&slotStatus[i], sizeof(slotStatus[i]));
    }
  }
  radio.stopListening();
}

void reserveSlot() {
  // Loop through the slot status array to find a free slot
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      if (slotStatus[i][j] == 0) { // If slot is free
        reservedSlotSet = i;
        reservedSlot = j;
        reservationTime = millis(); // Record the reservation time
        lcd.clear();
        // Display the reserved slot information
        displaySlotMessage(i + 1, (j / 2) + 1, (j % 2) + 1);
        entranceServo.write(135); // Open the entrance gate
        delay(5000); // Wait for the car to pass
        entranceServo.write(40); // Close the entrance gate
        return; // Exit the function once a slot is reserved
      }
    }
  }

  // If no free slot is found, display "Parking Full" message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Parking Full ");
  delay(1500);
  lcd.clear();
}

void checkReservationTimeout() {
  // If a slot is reserved and the reservation has timed out, reset the reservation
  if (reservedSlot != -1 && millis() - reservationTime > reservationTimeout) {
    reservedSlotSet = -1;
    reservedSlot = -1;
  }
}

void displaySlotMessage(int set, int parking, int slot) {
  // Display the reserved slot information on the LCD
  lcd.setCursor(0, 0);
  lcd.print(" Set ");
  lcd.print(set);
  lcd.print(" Parking ");
  lcd.print(parking);
  lcd.setCursor(0, 1);
  lcd.print(" Slot ");
  lcd.print(slot);
}

void displayWelcomeMessage() {
  // Display welcome message on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Welcome ");
}

void handleVehicleEntry() {
  // Handle vehicle entry by reserving a slot if not already reserved
  if (reservedSlot == -1) { // No slot is reserved
    reserveSlot();
  } else {
    // If a slot is already reserved, display "Slot Reserved" message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Slot Reserved ");
    delay(2000);
  }
}

void handleVehicleExit() {
  // Loop through the slot status array to find an occupied slot
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      if (slotStatus[i][j] == 1) { // If slot is occupied
        slotStatus[i][j] = 0; // Mark slot as free
        lcd.clear();
        // Display the exiting slot information
        displaySlotMessage(i + 1, (j / 2) + 1, (j % 2) + 1);
        exitServo.write(135); // Open the exit gate
        delay(5000); // Wait for the car to pass
        exitServo.write(40); // Close the exit gate
        return; // Exit the function once a slot is freed
      }
    }
  }
}
