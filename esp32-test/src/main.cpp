#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_HTU21DF.h"
#include "Adafruit_PM25AQI.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4 

//object list
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

void setup() {
  Serial.begin(115200);
  
 
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  delay(1000); 
  Wire.begin(21, 22); 
  Serial.println("\nBooting Air Monitor...");

  //oled ini
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED Failed!");
    while(1); 
  }

  //htu ini
  if (!htu.begin()) {
    Serial.println("HTU21D Failed!");
    while(1);
  }

  //pm2.5 ini
  if (!aqi.begin_UART(&Serial2)) {
    Serial.println("PM2.5 Sensor Failed! Check D16/D17 wiring.");
    while(1);
  }

  //oled display
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(15, 25);
  display.println("All Sensors GO!");
  display.display();
  delay(2000);
}

void loop() {

  float tempC = htu.readTemperature();
  float humidity = htu.readHumidity();


  PM25_AQI_Data data;
  bool aqiSuccess = aqi.read(&data);


  display.clearDisplay();

  // header
  display.setTextSize(1);
  display.setCursor(8, 0);
  display.println("Air Quality Station");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  // temp hum top row
  display.setCursor(0, 15);
  display.print("Temp: ");
  display.print(tempC, 1);
  display.print("C");
  
  display.setCursor(68, 15);
  display.print("RH: ");
  display.print(humidity, 1);
  display.print("%");

  // pm2.5 bot row
  display.setCursor(0, 32);
  display.print("PM 2.5 Level:");

  if (aqiSuccess) {
    display.setTextSize(2); 
    display.setCursor(0, 45);
    display.print(data.pm25_standard);
    
    display.setTextSize(1);
    display.print(" ug/m3");
  } else {
    display.setCursor(0, 45);
    display.print("Reading fan...");
  }

  // push to oled
  display.display(); 

/*
app json
*/
  if (aqiSuccess) {
    Serial.print("{\"temp\":");
    Serial.print(tempC, 1);
    Serial.print(",\"hum\":");
    Serial.print(humidity, 1);
    Serial.print(",\"pm25\":");
    Serial.print(data.pm25_standard);
    Serial.println("}");
  }

  delay(2000); 
}