# T-DisplayWeather
A sketch to display temperature, humidity, and pressure on a LILYGO T-Display-S3 using a BME280 sensor

## Prerequisites:

- [Adafruit BME280 library](https://github.com/adafruit/Adafruit_BME280_Library)
  - Sensor connects to HW I2C, pin 18 (SDA), pin 17 (SCL) for T-Display-S3
- [TFT_eSPI library](https://github.com/Bodmer/TFT_eSPI)
  - Configured for T-Display-S3
- [EasyButton Library](https://github.com/evert-arias/EasyButton)

## Hardware:
- LILYGO T-Display-S3
- BME280 sensor

## Setup:

1. Install the required libraries through the Arduino Library Manager.
2. Configure the TFT_eSPI library for T-Display-S3:
   - In the TFT_eSPI library folder, find the `User_Setup_Select.h` file.
   - Comment out the default setup file and uncomment the line for T-Display-S3.
3. Connect the BME280 sensor to the T-Display-S3:
   - VCC to 3.3V
   - GND to GND
   - SDA to GPIO 18
   - SCL to GPIO 17
4. Upload the sketch to your T-Display-S3.

## Features:
- Displays temperature in °C, humidity in %, and barometric pressure in inHg.
- Cycles through individual views for each measurement and a combined view.
- Uses the built-in button (GPIO 14) to manually change views.

## Notes:
- This sketch is specifically designed for the T-Display-S3 and may need adjustments for other boards.
- Make sure to select the correct board and port in the Arduino IDE before uploading.
- Temperature is displayed in Celsius. If you need Fahrenheit, you'll need to modify the code.
