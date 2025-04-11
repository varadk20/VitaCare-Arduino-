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
  int hrReadings[25];
  int spo2Readings[25];
  int count = 0;
  bool sensorStopped = false;

  void onBeatDetected() {
      if (count < 25) {
          hrReadings[count] = pox.getHeartRate();
          spo2Readings[count] = pox.getSpO2();
          count++;
      }
      if (count == 25) {
          sensorStopped = true;
          pox.shutdown();  // Stop the sensor
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
      }
      Serial.println("MAX30100 Initialized");
      lcd.setCursor(0, 1);
      lcd.print("Ready");

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

  bool alertTriggered = false; // Flag to ensure alert prints only once

  void loop() {
      if (!sensorStopped) {
          pox.update();
      }

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
              count = 0;
              sensorStopped = false;  // Reset for new readings
              alertTriggered = false; // Reset alert flag for new session
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Calculating...");
              pox.begin(); // Restart the sensor
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

      if (showReport && sensorStopped) {  // Ensure report is shown only after stopping
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("SpO2: ");
          lcd.print(spo2Readings[10]);
          lcd.print("%");
          lcd.setCursor(0, 1);
          lcd.print("BPM: ");
          lcd.print(hrReadings[10]);

          // Only print alert once
          if (!alertTriggered && (hrReadings[10] > 100 || hrReadings[10] < 60 || spo2Readings[10] < 95)) {
              Serial.println("ALERT1");
              alertTriggered = true; // Prevent multiple alerts
          }

          BTSerial.print("spo2: " + String(spo2Readings[10]) + "% BPM: " + String(hrReadings[10]));
          delay(5000);
      }

      if (showTemperature) {
          Wire.end();
          delay(10);
          Wire.begin();
          
          float tempF = mlx.readObjectTempF();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Temp: ");
          lcd.print(tempF);
          lcd.print(" F");

          if (tempF > 100.0){
            Serial.println("ALERT");
          } 

          if (tempF > 100.0) {
              BTSerial.print("High temperature: " + String(tempF) + " F");
          } else {
              BTSerial.print("Normal temperature: " + String(tempF) + " F");
          }

          delay(5000);
      }
  }
