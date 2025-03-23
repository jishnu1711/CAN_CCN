#include <SPI.h>
#include <mcp2515.h>

#define SCK 14   // SPI Clock
#define MISO 12  // SPI Master In Slave Out
#define MOSI 13  // SPI Master Out Slave In
#define CS 5     // Chip Select for MCP2515

#include <LiquidCrystal.h>
  
LiquidCrystal My_LCD(13, 12, 25,26,27,14);
 
MCP2515 mcp2515(CS); // Initialize MCP2515 with the CS pin

struct can_frame receivedMsg; // Struct for received messages
struct can_frame ackMsg;      // Struct for acknowledgment message

#define CAN_SENDER_ID_1 0x036  // Expected CAN ID from the sender
#define CAN_ACK_ID_1 0x037      // CAN ID for acknowledgment

#define CAN_SENDER_ID_2 0x038  // Expected CAN ID from the sender
#define CAN_ACK_ID_2 0x039   // CAN ID for acknowledgment

void setup() {
  Serial.begin(115200);
  mcp2515.reset();
  delay(100);  // Small delay after reset
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  My_LCD.begin(16, 2); 
  My_LCD.clear();

  Serial.println("Receiver ready!");
}

void loop() {
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
              My_LCD.print("uday");
              delay(500);  
            } 
        }
        if (receivedMsg.can_id == CAN_SENDER_ID_2) {
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
              My_LCD.print("reddy");
              My_LCD.setCursor(5, 1);
              My_LCD.print("challa");
              delay(1000);  
            } 
        }
        My_LCD.clear();
    }
}
