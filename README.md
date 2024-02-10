# arduino-humidity-fan-switch
Humidity fan switch that starts/stops fan based on a regulable humidity setpoint, the system also logs in a SD card.

# Device overview
![Top view](./images/top_view.png)

## List of materials
Link to store:
* [Arduino Uno](https://store-usa.arduino.cc/products/arduino-uno-rev3?selectedStore=us)
* SD card shield - Adafruit PID 1141
* [Enclosure](https://www.digikey.com/en/products/detail/bud-industries/PN-1336-DGMB/4897006)
* [Enclosure back plate](https://www.digikey.com/en/products/detail/bud-industries/NBX-10984-PL/2057387)
* LCD screen - Adafruit PID 292
* I2C LCD controller - Adafruit PID 292
* Humidity Sensor - Adafruit PID 5181
* Front panel buttons - Adafruit PID 1490
* [Fron panel LED](https://www.digikey.com/en/products/detail/lumex-opto-components-inc/SSI-LXH312GD-150/145100)
* [Cable glands](https://www.digikey.com/en/products/detail/bud-industries/IPG-2227/5291485)

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
