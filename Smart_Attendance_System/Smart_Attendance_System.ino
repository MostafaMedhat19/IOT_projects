#include "UbidotsESPMQTT.h"
#include <Firebase.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define RST_PIN         9           
#define SS_PIN          10         
#define TOKEN "BBUS-IIoL8c1xc5CGXMADdtBI3JPXhXUNK2"    
#define WIFINAME "....."  
#define WIFIPASS "....." 
#define REFERENCE_URL "https://rfiddooraccess-default-rtdb.firebaseio.com/"
#define AUTH_TOKEN "AIzaSyDnSBL8CRb6wsi2Ier5voQ4oFqv6sQ2guU"
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET -1

RTC_DS3231 rtc;
String Date = "";
int Counter = 0;
Ubidots client(TOKEN);
Firebase fb(REFERENCE_URL, AUTH_TOKEN);
MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
String UID = ""; 


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  DateTime now = rtc.now();
  Date = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day());
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Connecting to SSID\n'adafruit':");
  display.setCursor(0,0);
  display.display();
  
  Serial.begin(115200);
  SPI.begin();        
  mfrc522.PCD_Init();

 
  client.setDebug(true);  
  client.wifiConnection(WIFINAME, WIFIPASS);
  client.begin(callback);
  
  WiFi.disconnect();
  delay(1000);


  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }

  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFINAME, WIFIPASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("WiFi Connected");
}

void loop() {
  String path = "https://rfiddooraccess-default-rtdb.firebaseio.com/Class/Student1/name/.json"; 
  String name = fb.getString(path);
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }
  if (!client.connected()) {
    client.reconnect();
  }
   
  Serial.print(F("Card UID:"));
  UID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    UID += String(mfrc522.uid.uidByte[i], HEX);
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

 
  client.add("auth_access", Counter);  
  client.ubidotsPublish("esp32-project");
  client.loop();
  delay(5000);

 
  bool result = checkUid(UID); 
  if(result) {
  
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("Welcome " + name);
    display.display();
  } else {
    
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("Unknown UID");
    display.display();
  }
}

bool checkUid(String id) {
 
  String pathId = "/Class/Student1/" + id;
  String pathDate = "/Class/Student1/date/";

 
  int result = fb.setString(pathDate, Date); 

 
  int value = fb.getInt(pathId); 
  if (value == 1) {
    if (result == 200) {
      Counter++;  
      return true; 
    } else {
      return false; 
    }
  }
  
  return false; 
}
