
# VitaCare-Arduino

Sensor based smart health care system for rural and undeveloped places to provide vitals monitoring and provide important updates.


## Circuit diagram
Requirements:  
    1. Arduino Uno  
    2. LCD 16x02  
    3. HC05 bluetooth module  
    4. MLX90614 IR temperature  
    5. MAX30100 pulse & Spo2 sensor
    

![alt text](https://github.com/varadk20/VitaCare-Arduino-/blob/main/assets/circuit_image.png?raw=true)



## App Interface

App developed using mit app inventor  
Download the `VitaCare_Final.aia`and import in **MIT app 
Inventor** Then use app downloading apk or in companion mode.

* App features:
    * Voice commands
    * Send SMS updates
    * Reads vitals
    * English to Marathi translation


![alt text](https://github.com/varadk20/VitaCare-Arduino-/blob/main/assets/app_interface.png?raw=true)

## Usage
* Core Use:

    * Download the project and upload the `combine.ino` to Arduino.
    * Connect to `HC05` using the app.
    * From `Py_audio` folder run the `audio.py`. This will play the recorded precautions on abnormal readings of user.
    * Using `Translate` button in app we can translate text from english to Marathi.
    * We can send SMS of all details to users entering their phone number.

* ML Use:

    * Inside `DiseasePreditiction(ML)` folder download all files.
    * Download all python dependencies.
    * Run `cosine.py` for typing based symptom prediction.
    * For voice based run `cosine123.py`
