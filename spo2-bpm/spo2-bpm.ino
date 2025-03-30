  #include <Wire.h>
  #include <LiquidCrystal.h>
  #include "MAX30100_PulseOximeter.h"

  #define REPORTING_PERIOD_MS 1000

  // Initialize LCD (RS, EN, D4, D5, D6, D7)
  LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

  // Create PulseOximeter object
  PulseOximeter pox;

  int hrReadings[25]; // Array to store 25 heart rate readings
  int spo2Readings[25]; // Array to store 25 SpO2 readings
  int count = 0;    // Counter for readings

  // Callback when a pulse is detected
  void onBeatDetected() {
      Serial.print("Beat! ");
      Serial.print("Heart rate: ");
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
      lcd.begin(16, 2);
      lcd.setCursor(0, 0);
      lcd.print("Initializing...");

      // Initialize pulse oximeter
      if (!pox.begin()) {
          Serial.println("FAILED");
          lcd.setCursor(0, 1);
          lcd.print("Sensor Error!");
          while (1);
      } else {
          Serial.println("SUCCESS");
          lcd.setCursor(0, 1);
          lcd.print("Sensor Ready");
      }

      // Set LED current
      pox.setIRLedCurrent(MAX30100_LED_CURR_27_1MA);

      // Register callback
      pox.setOnBeatDetectedCallback(onBeatDetected);
  }

  void loop() {
      pox.update();
  }
