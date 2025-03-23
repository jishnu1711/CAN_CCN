#include <mcp_can.h>
#include <SPI.h>

#define CAN_CS_PIN 5      // MCP2515 Chip Select pin (Adjust as needed)
#define CAN_INT_PIN 2      // MCP2515 INT pin (Adjust based on wiring)

MCP_CAN CAN(CAN_CS_PIN);   // Create CAN object with CS pin

volatile bool Flag_Recv = false;
unsigned char len = 0;
unsigned char buf[8];

void IRAM_ATTR MCP2515_ISR() {  // Use IRAM_ATTR for ESP32 ISR
    Flag_Recv = true;
}

void setup() {
  Serial.begin(115200);
  
  // Initialize CAN-BUS
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN initialized successfully!");
  } else {
    Serial.println("CAN initialization failed!");
    while (1);
  }
  
  CAN.setMode(MCP_NORMAL);  // Set CAN to normal mode

  pinMode(CAN_INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), MCP2515_ISR, FALLING); // Correct interrupt pin
}

void loop() {
  if (Flag_Recv) {  // Check if data received
    Flag_Recv = false;  // Clear flag
    CAN.readMsgBuf(&len, buf);  // Read CAN message

    Serial.println("CAN_BUS RECEIVED DATA:");
    Serial.print("Data Length = ");
    Serial.println(len);
      
    for (int i = 0; i < len; i++) {  // Print received data
      Serial.print(buf[i]);
      Serial.print("\t");
    }
    Serial.println();
  }
}
