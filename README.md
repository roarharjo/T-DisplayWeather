# T-DisplayWeather
A sketch to display time, temperature, humidity, and pressure on a LILYGO T-Display-S3 using a BME280 sensor and Wi-Fi for time synchronization.

## Prerequisites:

- [Adafruit BME280 library](https://github.com/adafruit/Adafruit_BME280_Library)
- [TFT_eSPI library](https://github.com/Bodmer/TFT_eSPI)
- [EasyButton Library](https://github.com/evert-arias/EasyButton)
- WiFi library (built-in with ESP32)

## Features:
- Connects to Wi-Fi and synchronizes time with NTP server
- Displays current time, temperature in °C, humidity in %, and barometric pressure in inHg
- Cycles through individual views for each measurement and a combined view
- Uses the built-in button (GPIO 14) to manually change views

## Setup:
1. Install the required libraries
2. Configure the TFT_eSPI library for T-Display-S3
3. Set your Wi-Fi credentials in the sketch
4. Set your timezone offsets in the sketch
5. Upload the sketch to your T-Display-S3

## Notes:
- Make sure you have a stable internet connection for initial time synchronization
- The clock updates every second, while sensor readings update every minute
