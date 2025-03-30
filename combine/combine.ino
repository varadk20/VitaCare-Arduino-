#include <Wire.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>

#define REPORTING_PERIOD_MS 1000

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
SoftwareSerial BTSerial(10, 11);
PulseOximeter pox;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

bool showTemperature = false;
bool showReport = false;
unsigned long lastReportTime = 0;
int hrReadings[25]; // Array to store 25 heart rate readings
int spo2Readings[25]; // Array to store 25 SpO2 readings
int count = 0;

void onBeatDetected() {
    Serial.print("Beat! HR: ");
    Serial.print(pox.getHeartRate());
    Serial.print(" bpm / SpO2: ");
    Serial.print(pox.getSpO2());
    Serial.println("%");

    if (count < 25) {
          hrReadings[count] = pox.getHeartRate(); // Store HR value
          spo2Readings[count] = pox.getSpO2(); // Store SpO2 value
          count++;
      }

    if (count == 25) {
        int sumHR = 0, sumSpO2 = 0;
        for (int i = 5; i < 25; i++) { // Ignore first 5 readings
            sumHR += hrReadings[i];
            sumSpO2 += spo2Readings[i];
        }
        int avgHeartRate = sumHR / 20; // Compute average HR
        int avgSpO2 = sumSpO2 / 20; // Compute average SpO2

        // Display on LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Avg HR: ");
        lcd.print(avgHeartRate);
        lcd.print(" bpm");

        lcd.setCursor(0, 1);
        lcd.print("Avg SpO2: ");
        lcd.print(avgSpO2);
        lcd.print("%");

        count++; // Prevent further averaging
    }
}

void setup() {
    Serial.begin(9600);
    BTSerial.begin(9600);
    Wire.begin();
    
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    
    if (!pox.begin()) {
        Serial.println("MAX30100 failed!");
        lcd.setCursor(0, 1);
        lcd.print("Sensor Error!");
        while (1);
    }else {
          Serial.println("SUCCESS");
          lcd.setCursor(0, 1);
          lcd.print("Sensor Ready");
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_27_1MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);
    
    if (!mlx.begin()) {
        Serial.println("MLX90614 failed!");
        lcd.setCursor(0, 1);
        lcd.print("Temp Sensor Err");
        while (1);
    }
    lcd.setCursor(0, 1);
    lcd.print("Ready");
}

void loop() {
    pox.update();
    
    if (BTSerial.available()) {
        String receivedText = "";
        while (BTSerial.available()) {
            receivedText += (char)BTSerial.read();
            delay(10);
        }
        receivedText.trim();
        
        lcd.clear();
        lcd.setCursor(0, 0);
        
        if (receivedText.equalsIgnoreCase("status")) {
            showTemperature = true;
            showReport = false;
        } else if (receivedText.equalsIgnoreCase("report")) {
            showTemperature = false;
            showReport = true;
        } else {
            showTemperature = false;
            showReport = false;
            lcd.print(receivedText.substring(0, 16));
            if (receivedText.length() > 16) {
                lcd.setCursor(0, 1);
                lcd.print(receivedText.substring(16, 32));
            }
        }
    }
    
    if (showTemperature) {
        Wire.end();  // Restart I2C to prevent conflict
        delay(10);
        Wire.begin();
        
        float tempF = mlx.readObjectTempF();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(tempF);
        lcd.print(" F");
        delay(1000);
    }
    
}
