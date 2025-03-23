#include <SPI.h>
#include <mcp2515.h>

#define SCK     14  // Clock
#define MISO    12  // Master In Slave Out
#define MOSI    13  // Master Out Slave In
#define CS      5   // Chip Select (change if needed)

MCP2515 mcp2515(CS); // Define MCP2515 with CS pin

struct can_frame canMsg;
struct can_frame receivedMsg;  // Separate struct for reading

#define MAX_RETRIES 10
#define CAN_ACK_ID 0x037  // CAN ID for acknowledgment

void setup() {
  Serial.begin(115200);
  
  // Initialize SPI with defined pins
  //SPI.begin(SCK, MISO, MOSI, CS);
  
  mcp2515.reset();
  delay(100); // Small delay after reset
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ); // Change to MCP_16MHZ if needed
  mcp2515.setNormalMode();
}

void loop() {
  canMsg.can_id  = 0x036;  // CAN ID
  canMsg.can_dlc = 2;      // Data length code (number of bytes)
  canMsg.data[0] = 3;
  canMsg.data[1] = 2;

  bool messageSent = false;
  int retries = 0;

  while (!messageSent && retries < MAX_RETRIES) {
    if (mcp2515.sendMessage(&canMsg) == MCP2515::ERROR_OK) {
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
        Serial.println("ACK received");
        messageSent = true;
      } else {
        Serial.println("ACK not received, retrying...");
        retries++;
      }
    } else {
      Serial.println("Error sending message, retrying...");
      retries++;
    }
  }

  if (!messageSent) {
    Serial.println("Failed to send message after retries");
  }

  delay(1000); // Send data every second
}
