# arduino-humidity-fan-switch
Humidity fan switch that starts/stops fan based on a regulable humidity setpoint, the system also logs in a SD card.

# Device overview
![Top view](./images/top_view.png)

## List of materials
Link to store:
* Arduino Uno
* SD card shield
* Enclosure
* Enclosure back plate
* LCD screen
* LCD I2C controller
* Humidity Sensor
* Front panel buttons
* Fron panel LED
* Cable glands

Other materials:
* M2.5 screws
* Protoboard
* Single wires
* RJ45 cable to interface LCD with main board
* USB cable connected to Arduino Uno board

## Arduino code dependencies
List of libraries you have to install in Arduino IDE to run the code:
* Adafruit RTC Lib
* Adafruit LiquidCrystal
* Adafruit AHTX0
