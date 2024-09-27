// Weather sensor program for TTGO T-Display w/onboard TFT + BME280 sensor
#define FS_NO_GLOBALS
#define SPWIDTH  240                 // sprite width
#define SPHEIGHT 135                 // sprite height

#include <Adafruit_BME280.h>         // Include the BME280 library (works with 2.0.2+)
#include <TFT_eSPI_S3.h>             // Include the graphics library for T-Display-S3
#include <EasyButton.h>              // Include the button handler
#include "Free_Fonts.h"              // Reference to the free fonts header
#include <WiFi.h>
#include <time.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;  // Replace with your GMT offset (seconds)
const int   daylightOffset_sec = 3600;  // Replace with your daylight offset (seconds)

//Global variables for displayed values
float Pa;                            
float Baro;
float cTemp, cTempHi, cTempLo;  // Changed from cTempF to cTemp
float Humid;
float HumidHi;
float HumidLo;
float SLPressure;
int Elevation = 549;                  // Elevation of current location in Meters
int displayMode = 1;
char cTempF[10];
char cTempHi[10];
char cTempLo[10];
char cBaro[10];
char cHumid[10];
char cHumidHi[10];
char cHumidLo[10];
long lastUpdate = 0;

// Initialize devices
Adafruit_BME280 bme;                  // BME sensor instance
TFT_eSPI_S3 tft = TFT_eSPI_S3();      // tft instance for T-Display-S3
TFT_eSprite spr = TFT_eSprite(&tft);  // configure sprites for flicker free updates
TFT_eSprite spr1 = TFT_eSprite(&tft);
EasyButton button1(14);               // setup button on pin 14 for T-Display-S3

void setup() {

  tft.begin();                        // Enable the display
  tft.setRotation(3);                 // set display rotation to landscape
  bme.begin(0x76, &Wire);             // Enable the bme280
 
  if (!SPIFFS.begin()) {              // init SPIFFS
    while (1) yield();                // Stay here twiddling thumbs waiting for SPIFFS
  }
  button1.begin();
  button1.onPressed(onPress1);
  //initial values for hi/lo 
  cTempHi = 0;
  cTempLo = 150;
  HumidHi = 0;
  HumidLo = 100;
  tft.fillScreen(TFT_BLACK);          // clear the screen
  drawScreen();                       // initial draw of display

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
//end setup



void readSensors() {

  cTemp = bme.readTemperature();      // read temp in Celsius
  Pa = bme.readPressure();            // read the pressure in Pascals
  Humid = bme.readHumidity();         // read humidity in percent
  SLPressure = (Pa) / pow((1 - ((float)(Elevation)) / 44330), 5.255); // Calculates sea level reference pressure from measured value and Elevation
  Baro = SLPressure / 3386.39;        // corrected barometric pressure
  // store hi and lo values
  if (Humid > HumidHi) HumidHi = Humid;
  if (Humid < HumidLo) HumidLo = Humid; 
  if (cTemp > cTempHi) cTempHi = cTemp;
  if (cTemp < cTempLo) cTempLo = cTemp; 
  // convert float to string
  dtostrf(cTemp, 2, 0, cTempF);
  dtostrf(cTempHi, 2, 0, cTempHi);
  dtostrf(cTempLo, 2, 0, cTempLo);
  dtostrf(Baro, 4, 2, cBaro);
  dtostrf(Humid, 2, 0, cHumid);
  dtostrf(HumidHi, 2, 0, cHumidHi);
  dtostrf(HumidLo, 2, 0, cHumidLo);    
}

// end readSensors

void onPress1() {                     // do something when the button is pressed
  displayMode ++;                     // increment the display mode
  drawScreen();                       // draw the screen
}
//end onPress1

void writeTemp() {
  String fontLarge = "Interceptor72";
  String fontMedium = "Interceptor36";
  
  spr.createSprite(SPWIDTH, SPHEIGHT);
  spr.fillSprite(TFT_BLACK);
  
  // Large temperature display
  spr.setTextColor(TFT_WHITE);
  spr.loadFont(fontLarge);
  spr.drawRightString(cTempF, SPWIDTH - 10, 10, GFXFF);
  spr.unloadFont();
  
  // °C symbol
  spr.setTextColor(TFT_CYAN);
  spr.setTextFont(4);
  spr.drawString("°C", SPWIDTH - 5, 5);
  
  // Hi/Lo temperatures
  spr.setTextColor(TFT_LIGHTGREY);
  spr.loadFont(fontMedium);
  spr.drawString("Hi: " + String(cTempHi) + "°  Lo: " + String(cTempLo) + "°", 10, SPHEIGHT - 40, GFXFF);
  spr.unloadFont();
  
  spr.pushSprite(0, 0);
  spr.deleteSprite();
}

void writeHumid() {

  // Draw Sensor Output

  String fontSmall = "Interceptor28";
  String fontLarge  = "Interceptor72";
  spr.createSprite(SPWIDTH, SPHEIGHT);
  spr.fillSprite(TFT_BLACK);
  spr.setTextColor(TFT_RED);
  spr.setTextFont(2);
  spr.setCursor(0, 20);
  spr.print("Humidity (%)");
  spr.setCursor(0, 73);
  spr.print("Hi:");
  spr.setCursor(0, 113);
  spr.print("Lo:");
  spr.drawFastHLine(0, 30, 239, TFT_CYAN);
  spr.drawFastVLine(95, 30, 105, TFT_CYAN);
  spr.setTextColor(TFT_CYAN);
  spr.loadFont(fontSmall);
  spr.drawRightString(cHumidHi, 85, 55, GFXFF);
  spr.drawRightString(cHumidLo, 85, 95, GFXFF);
  spr.unloadFont();
  spr.loadFont(fontLarge);
  spr.drawRightString(cHumid, 239, 60, GFXFF);
  spr.unloadFont();
  spr.pushSprite(0, 0);
  spr.deleteSprite();
}

void writeBaro() {

  // Draw Sensor Output

  String fontSmall = "Interceptor28";
  String fontLarge  = "Interceptor72";
  spr.createSprite(SPWIDTH, SPHEIGHT);
  spr.fillSprite(TFT_BLACK);
  spr.setTextColor(TFT_RED);
  spr.setTextFont(2);
  spr.setCursor(0, 20);
  spr.print("Pressure (inHg)");
  spr.drawFastHLine(0, 30, 240, TFT_CYAN);
  spr.setTextColor(TFT_CYAN);
  spr.loadFont(fontLarge);
  spr.drawRightString(cBaro, 119, 60, GFXFF);
  spr.unloadFont();
  spr.pushSprite(0, 0, TFT_TRANSPARENT);
  spr.deleteSprite();
}

void transHumid() {

  // Draw Sensor Output

  String fontSmall = "Interceptor28";
  String fontLarge  = "Interceptor72";
  spr.createSprite(SPWIDTH, SPHEIGHT);
  spr.setTextColor(TFT_RED);
  spr.setTextFont(2);
  spr.setCursor(0, 20);
  spr.print("Temperature (°C)");
  spr.setCursor(0, 73);
  spr.print("Hi:");
  spr.setCursor(0, 113);
  spr.print("Lo:");
  spr.drawFastHLine(0, 30, 239, TFT_CYAN);
  spr.drawFastVLine(95, 30, 105, TFT_CYAN);
  spr.setTextColor(TFT_CYAN);
  spr.loadFont(fontSmall);
  spr.drawRightString(cTempHi, 85, 55, GFXFF);
  spr.drawRightString(cTempLo, 85, 95, GFXFF);
  spr.unloadFont();
  spr.loadFont(fontLarge);
  spr.drawRightString(cTempF, 239, 60, GFXFF);
  spr.unloadFont();
  spr1.createSprite(SPWIDTH, SPHEIGHT);
  spr1.setTextColor(TFT_RED);
  spr1.setTextFont(2);
  spr1.setCursor(0, 20);
  spr1.print("Humidity (%)");
  spr1.setCursor(0, 73);
  spr1.print("Hi:");
  spr1.setCursor(0, 113);
  spr1.print("Lo:");
  spr1.drawFastHLine(0, 30, 239, TFT_CYAN);
  spr1.drawFastVLine(95, 30, 105, TFT_CYAN);
  spr1.setTextColor(TFT_CYAN);
  spr1.loadFont(fontSmall);
  spr1.drawRightString(cHumidHi, 85, 55, GFXFF);
  spr1.drawRightString(cHumidLo, 85, 95, GFXFF);
  spr1.unloadFont();
  spr1.loadFont(fontLarge);
  spr1.drawRightString(cHumid, 239, 60, GFXFF);
  spr1.unloadFont();
  for (int ypos = SPHEIGHT; ypos > 0; ypos -= 2) {
    spr.pushSprite(0, (ypos - 2) - SPHEIGHT);
    spr1.pushSprite(0, ypos - 2);
  }
  spr.deleteSprite();
  spr1.deleteSprite();
}

void transBaro() {

  // Draw Sensor Output

  String fontSmall = "Interceptor28";
  String fontLarge  = "Interceptor72";
  spr.createSprite(SPWIDTH, SPHEIGHT);
  spr.fillSprite(TFT_BLACK);
  spr.setTextColor(TFT_RED);
  spr.setTextFont(2);
  spr.setCursor(0, 20);
  spr.print("Humidity (%)");
  spr.setCursor(0, 73);
  spr.print("Hi:");
  spr.setCursor(0, 113);
  spr.print("Lo:");
  spr.drawFastHLine(0, 30, 239, TFT_CYAN);
  spr.drawFastVLine(95, 30, 105, TFT_CYAN);
  spr.setTextColor(TFT_CYAN);
  spr.loadFont(fontSmall);
  spr.drawRightString(cHumidHi, 85, 55, GFXFF);
  spr.drawRightString(cHumidLo, 85, 95, GFXFF);
  spr.unloadFont();
  spr.loadFont(fontLarge);
  spr.drawRightString(cHumid, 239, 60, GFXFF);
  spr.unloadFont();
  spr1.createSprite(SPWIDTH, SPHEIGHT);
  spr1.fillSprite(TFT_BLACK);
  spr1.setTextColor(TFT_RED);
  spr1.setTextFont(2);
  spr1.setCursor(0, 20);
  spr1.print("Pressure (inHg)");
  spr1.drawFastHLine(0, 30, 240, TFT_CYAN);
  spr1.setTextColor(TFT_CYAN);
  spr1.loadFont(fontLarge);
  spr1.drawRightString(cBaro, 119, 60, GFXFF);
  spr1.unloadFont();
  for (int ypos = SPHEIGHT; ypos > 0; ypos -= 2) {
    spr.pushSprite(0, (ypos - 2) - SPHEIGHT);
    spr1.pushSprite(0, ypos - 2);
  }
  spr.deleteSprite();
  spr1.deleteSprite();
}

void transTemp() {
  // Draw Sensor Output

  String fontSmall = "Interceptor28";
  String fontLarge  = "Interceptor72";
  spr.createSprite(SPWIDTH, SPHEIGHT);
  spr.fillSprite(TFT_BLACK);
  spr.setTextColor(TFT_RED);
  spr.setTextFont(2);
  spr.setCursor(0, 20);
  spr.print("Pressure (inHg)");
  spr.drawFastHLine(0, 30, 240, TFT_CYAN);
  spr.setTextColor(TFT_CYAN);
  spr.loadFont(fontLarge);
  spr.drawRightString(cBaro, 119, 60, GFXFF);
  spr.unloadFont();
  spr1.createSprite(SPWIDTH, SPHEIGHT);
  spr1.setTextColor(TFT_RED);
  spr1.setTextFont(2);
  spr1.setCursor(0, 20);
  spr1.print("Temperature (°C)");
  spr1.setCursor(0, 73);
  spr1.print("Hi:");
  spr1.setCursor(0, 113);
  spr1.print("Lo:");
  spr1.drawFastHLine(0, 30, 239, TFT_CYAN);
  spr1.drawFastVLine(95, 30, 105, TFT_CYAN);
  spr1.setTextColor(TFT_CYAN);
  spr1.loadFont(fontSmall);
  spr1.drawRightString(cTempHi, 85, 55, GFXFF);
  spr1.drawRightString(cTempLo, 85, 95, GFXFF);
  spr1.unloadFont();
  spr1.loadFont(fontLarge);
  spr1.drawRightString(cTempF, 239, 60, GFXFF);
  spr1.unloadFont();
  for (int ypos = SPHEIGHT; ypos > 0; ypos -= 2) {
    spr.pushSprite(0, (ypos - 2) - SPHEIGHT);
    spr1.pushSprite(0, ypos - 2);
  }
  spr.deleteSprite();
  spr1.deleteSprite();
}

void drawCombinedView() {
  spr.fillSprite(TFT_BLACK);
  
  // Time
  spr.setTextColor(TFT_YELLOW);
  spr.setFreeFont(FSS12);
  spr.drawString(getTimeString(), 10, 10, GFXFF);
  
  // Temperature
  spr.setTextColor(TFT_WHITE);
  spr.setFreeFont(FSS18);
  spr.drawString(String(cTemp, 1) + "°C", 10, 40, GFXFF);
  
  // Humidity
  spr.setTextColor(TFT_CYAN);
  spr.setFreeFont(FSS18);
  spr.drawString(String(cHumid) + "% RH", 10, 80, GFXFF);
  
  // Pressure
  spr.setTextColor(TFT_GREEN);
  spr.setFreeFont(FSS18);
  spr.drawString(String(cBaro) + " inHg", 10, 120, GFXFF);
  
  spr.pushSprite(0, 0);
}

void drawScreen() {

   readSensors();                          // read the sensors and calculate values

   if (displayMode > 6) displayMode = 1;   // roll over the display mode back to 1

  switch (displayMode) {                   // select display mode
    case 1: writeTemp(); break;
    case 2: transHumid(); displayMode = 3; break;
    case 3: writeHumid(); break;
    case 4: transBaro(); displayMode = 5; break;
    case 5: writeBaro(); break;
    case 6: drawCombinedView(); displayMode = 1; break;
  }
}

void loop() {
  button1.read();
  
  static unsigned long lastDisplayUpdate = 0;
  static unsigned long lastSensorUpdate = 0;
  unsigned long currentMillis = millis();

  // Update display every second for clock
  if (currentMillis - lastDisplayUpdate >= 1000) {
    drawScreen();
    lastDisplayUpdate = currentMillis;
  }

  // Update sensor readings every minute
  if (currentMillis - lastSensorUpdate >= 60000) {
    getReadings();
    lastSensorUpdate = currentMillis;
  }
}

//end loop

String getTimeString() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "00:00:00";
  }
  char timeStringBuff[9]; //HH:MM:SS + null terminator
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);
  return String(timeStringBuff);
}
