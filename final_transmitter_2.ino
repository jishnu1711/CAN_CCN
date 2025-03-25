#include <SPI.h>
#include <mcp2515.h>
#include <ESP32Servo.h>

Servo servo_1;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 26;

#define SCK     14  // Clock
#define MISO    12  // Master In Slave Out
#define MOSI    13  // Master Out Slave In
#define CS      5   // Chip Select (change if needed)
#define button_pin 15

MCP2515 mcp2515(CS); // Define MCP2515 with CS pin

struct can_frame canMsg2;
struct can_frame receivedMsg;  // Separate struct for reading

// struct can_frame receivedMsg; // Struct for received messages
struct can_frame ackMsg;  

#define MAX_RETRIES 10
#define CAN_ACK_ID 0x039  // CAN ID for acknowledgment

#define CAN_SENDER_ID 0x032
#define CAN_ACK_ID_r2 0x033

void setup() {
  Serial.begin(115200);
  mcp2515.reset();
  delay(100); // Small delay after reset
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ); // Change to MCP_16MHZ if needed
  mcp2515.setNormalMode();
  pinMode(button_pin, INPUT_PULLUP);  // Use internal pull-up resistor
  servo_1.attach(servoPin);
  servo_1.write(33);
}

void loop() {
  canMsg2.can_id  = 0x038;  // CAN ID
  canMsg2.can_dlc = 1;      // Data length code (number of bytes)
  canMsg2.data[0] = 1;

  bool messageSent = false;
  int retries = 0;

  if (mcp2515.readMessage(&receivedMsg) == MCP2515::ERROR_OK) { // Check if a message is received
    Serial.print("Received CAN ID: 0x");
    Serial.println(receivedMsg.can_id, HEX);
    // If the received message is from the expected sender
    if (receivedMsg.can_id == CAN_SENDER_ID) {
      Serial.println("Valid message received! Sending ACK...");
      // Prepare acknowledgment message
      ackMsg.can_id = CAN_ACK_ID_r2;
      ackMsg.can_dlc = 1;      // Data length of 1 byte
      ackMsg.data[0] = 1;      // ACK data
      // Send ACK message
      if (mcp2515.sendMessage(&ackMsg) == MCP2515::ERROR_OK) {
          Serial.println("ACK sent successfully!");
      } else {
          Serial.println("Failed to send ACK.");
      }
      for (int i = 0; i < receivedMsg.can_dlc; i++) {
      Serial.print(receivedMsg.data[i], DEC);  // Print each byte as an integer
      Serial.print(" ");
      }
      if(receivedMsg.data[0]=1){
        Serial.println("switching servo on!");
        servo_1.write(90);
        delay(200);
        servo_1.write(0);
        delay(200);
      } 
    }
  }
  int a = digitalRead(button_pin);
 if (a == 0) {  // Active-low button press
    Serial.println("Button2 is pressed");
    while (!messageSent && retries < MAX_RETRIES) {
    if (mcp2515.sendMessage(&canMsg2) == MCP2515::ERROR_OK) {
      unsigned long startTime = millis();
      bool ackReceived = false;

      while (millis() - startTime < 1000) { // Wait up to 500ms for an ACK
        if (mcp2515.readMessage(&receivedMsg) == MCP2515::ERROR_OK) { // Use separate variable
          if (receivedMsg.can_id == CAN_ACK_ID) { // Check correct CAN ID
            ackReceived = true;
            break;
          }
        }
      }
      if (ackReceived) {
        Serial.println("ACK received from display");
        messageSent = true;
      } else {
        Serial.println("ACK not received, retrying...");
        retries++;
      }
      } else {
        Serial.println("Error sending message, retrying...");
        retries++;
      }
      if (!messageSent) {
        Serial.println("Failed to send message after retries");
      }
    }
  }
}
