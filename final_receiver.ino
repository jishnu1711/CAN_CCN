#include <SPI.h>
#include <mcp2515.h>

#define SCK 14   // SPI Clock
#define MISO 12  // SPI Master In Slave Out
#define MOSI 13  // SPI Master Out Slave In
#define CS 5     // Chip Select for MCP2515

#define button_pin_1 22
#define button_pin_2 21

#include <LiquidCrystal.h>
  
LiquidCrystal My_LCD(13, 12, 25,26,27,14);
 
MCP2515 mcp2515(CS); // Initialize MCP2515 with the CS pin

struct can_frame receivedMsg; // Struct for received messages
struct can_frame ackMsg;      // Struct for acknowledgment message

#define CAN_SENDER_ID_1 0x036  // Expected CAN ID from the sender
#define CAN_ACK_ID_1 0x037      // CAN ID for acknowledgment

#define CAN_SENDER_ID_2 0x038  // Expected CAN ID from the sender
#define CAN_ACK_ID_2 0x039   // CAN ID for acknowledgment

#define CAN_ACK_ID_r1 0x035      // CAN ID for acknowledgment
#define CAN_ACK_ID_r2 0x033
struct can_frame canMsg1;
struct can_frame canMsg2;
#define MAX_RETRIES 3

void setup() {
  Serial.begin(115200);
  mcp2515.reset();
  delay(100);  // Small delay after reset
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  My_LCD.begin(16, 2); 
  My_LCD.clear();

  pinMode(button_pin_1, INPUT_PULLUP);  // Use internal pull-up resistor
  pinMode(button_pin_2, INPUT_PULLUP);  // Use internal pull-up resistor

  Serial.println("Receiver ready!");
}

void loop() {
  canMsg1.can_id  = 0x034;  // CAN ID
  canMsg1.can_dlc = 1;      // Data length code (number of bytes)
  canMsg1.data[0] = 1;

  canMsg2.can_id  = 0x032;  // CAN ID
  canMsg2.can_dlc = 1;      // Data length code (number of bytes)
  canMsg2.data[0] = 1;

  bool messageSent = false;
  int retries = 0;

  if (mcp2515.readMessage(&receivedMsg) == MCP2515::ERROR_OK) { // Check if a message is received
    Serial.print("Received CAN ID: 0x");
    Serial.println(receivedMsg.can_id, HEX);
    // If the received message is from the expected sender
    if (receivedMsg.can_id == CAN_SENDER_ID_1) {
      Serial.println("Valid message received! Sending ACK...");
      // Prepare acknowledgment message
      ackMsg.can_id = CAN_ACK_ID_1;
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
        My_LCD.print("Turn on Motor1");
        delay(500);  
      } 
    }
    else if (receivedMsg.can_id == CAN_SENDER_ID_2) {
      Serial.println("Valid message received! Sending ACK...");
      // Prepare acknowledgment message
      ackMsg.can_id = CAN_ACK_ID_2;
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
        My_LCD.print("Turn on Motor2");
        delay(1000);  
      } 
    }
    My_LCD.clear();
  }
  // else{
    int a = digitalRead(button_pin_1);
    int b = digitalRead(button_pin_2);
    if(a == 0) {  // Active-low button press
      Serial.println("Button on main is pressed");
      while (!messageSent && retries < MAX_RETRIES) {
        if (mcp2515.sendMessage(&canMsg1) == MCP2515::ERROR_OK) {
          unsigned long startTime = millis();
          bool ackReceived = false;

          while (millis() - startTime < 1000) { // Wait up to 500ms for an ACK
            if (mcp2515.readMessage(&receivedMsg) == MCP2515::ERROR_OK) { // Use separate variable
              if (receivedMsg.can_id == CAN_ACK_ID_r1) { // Check correct CAN ID
                ackReceived = true;
                break;
              }
            }
          }
          if (ackReceived) {
            Serial.println("ACK received from a");
            My_LCD.clear();
            My_LCD.setCursor(0,0);
            My_LCD.print("Motor1 should");
            My_LCD.setCursor(0,1);
            My_LCD.print("be rotating");
            messageSent = true;
          } else {
            Serial.println("ACK not received, retrying...");
            retries++;
          }
        } 
        else {
            Serial.println("Error sending message, retrying...");
            retries++;
        }
        if (!messageSent) {
          Serial.println("Failed to send message after retries");
        }
      }
    }
    
    else if (b == 0) {  // Active-low button press
      Serial.println("Button on main is pressed");
      while (!messageSent && retries < MAX_RETRIES) {
        if (mcp2515.sendMessage(&canMsg2) == MCP2515::ERROR_OK) {
          unsigned long startTime = millis();
          bool ackReceived = false;

          while (millis() - startTime < 1000) { // Wait up to 500ms for an ACK
            if (mcp2515.readMessage(&receivedMsg) == MCP2515::ERROR_OK) { // Use separate variable
              if (receivedMsg.can_id == CAN_ACK_ID_r2) { // Check correct CAN ID
                ackReceived = true;
                break;
              }
            }
          }
          if (ackReceived) {
            Serial.println("ACK received from b");
            My_LCD.clear();
            My_LCD.setCursor(0,0);
            My_LCD.print("Motor2 should");
            My_LCD.setCursor(0,1);
            My_LCD.print("be rotating");
            messageSent = true;
          } else {
            Serial.println("ACK not received, retrying...");
            retries++;
          }
        } 
        else {
            Serial.println("Error sending message, retrying...");
            retries++;
        }
        if (!messageSent) {
          Serial.println("Failed to send message after retries");
        }
      }
    }
  // }
}
