#include <LiquidCrystal.h>
 
// Create An LCD Object. Signals: [ RS, EN, D4, D5, D6, D7 ] 
LiquidCrystal My_LCD(13, 12, 25,26,27,14);
 
void setup() 
{
  // Initialize The LCD. Parameters: [ Columns, Rows ] 
  My_LCD.begin(16, 2); 
  My_LCD.clear();
  // Clears The LCD Display 
 
}
 
void loop() 
{
   
 
  // Display The First Message In Home Position (0, 0) 
  My_LCD.print("uday"); 
  // Set The Cursor Position To: [ Col 5, Row 1] 
  // The Next Message Will Start From The 6th Char Position in The 2nd Row 
  // Note: 1st Row Has An Index of 0, The 2nd Row Has An Index of 1 
  My_LCD.setCursor(5, 1); 
  // Display The Second Message In Position (5, 1) 
  My_LCD.print("GG izi");
  delay(1000);
}