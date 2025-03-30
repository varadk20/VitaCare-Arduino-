#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
SoftwareSerial BTSerial(10, 11); // RX, TX for Bluetooth
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // RS, E, D4, D5, D6, D7



bool showTemperature = false; 

void setup() {
    Serial.begin(9600);
    BTSerial.begin(9600);
    mlx.begin();
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Waiting for data");
  
}

void loop() {
                                               

    // Check if new Bluetooth data is received
    if (BTSerial.available()) {
        String receivedText = "";
        while (BTSerial.available()) {
          receivedText += (char)BTSerial.read();
          delay(10);
        }

        // Serial.print("Received: "); 
        // Serial.println(receivedText);
        
        
        lcd.clear();
        lcd.setCursor(0, 0);
        
        if (receivedText.equalsIgnoreCase("status")) {
          showTemperature = true;
        } else {
            showTemperature = false; // Stop temperature updates
            lcd.print(receivedText.substring(0, 16)); // Print first 16 characters
            if (receivedText.length() > 16) {
                lcd.setCursor(0, 1);
                lcd.print(receivedText.substring(16, 32)); // Print second line
            }
        }
    }

    if (showTemperature) {
        float tempF = mlx.readObjectTempF(); 
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(tempF);
        lcd.print(" F");
  
        delay(1000); // Update every 1 second
    }

}
