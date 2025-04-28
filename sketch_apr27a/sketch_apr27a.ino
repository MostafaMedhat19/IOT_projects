#include <SPI.h>
#include <Firebase.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "UbidotsESPMQTT.h"

const char* REFERENCE_URL = "https://rfiddooraccess-default-rtdb.firebaseio.com/";
const char* AUTH_TOKEN = "6tFsRuLvPmfgFJMVQFF8UeTqTs3DS88WbDysDK8B";
const int RST_PIN = 5;   
const int SS_PIN = 2;
const char* TOKEN = "BBUS-IIoL8c1xc5CGXMADdtBI3JPXhXUNK2";    
char* WIFI_SSID = "Alaa"; 
char* WIFI_PASSWORD = "123456789";
const int BUZZER = 27;
const int BLUE_LED = 12;
const int RED_LED = 14;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

Ubidots client((char*)TOKEN);
MFRC522 mfrc522(SS_PIN, RST_PIN); 
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);
Firebase fb(REFERENCE_URL, AUTH_TOKEN);
String UID = ""; 
int counter = 0; 

bool checkUid(String id) {
   String path = "users/" + id; 
   int value = fb.getInt(path); 
   if (!value) {
      return false;
   }
   counter++; 
   return true; 
}

void showMessage(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(msg);
  display.display(); 
}

void pushLogs(String id, String message) {
   fb.pushString("logs", id + ":" + message);
}

void setup() {
  Serial.begin(115200);  
  while (!Serial);    
  SPI.begin();       
  mfrc522.PCD_Init(); 
  pinMode(BUZZER, OUTPUT); 
  pinMode(BLUE_LED, OUTPUT); 
  pinMode(RED_LED, OUTPUT); 
  digitalWrite(BUZZER, LOW); 
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(RED_LED, LOW);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Connecting...");
  display.display(); 

  Serial.print("Connecting to: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi Connected");
  client.setDebug(true);  
  client.wifiConnection(WIFI_SSID, WIFI_PASSWORD);
  client.begin(callback);
}

void loop() {
  if (!client.connected()) {
    client.reconnect();
  }
  
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  UID = ""; 
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    UID += String(mfrc522.uid.uidByte[i]);
  } 

  Serial.print("Card UID: ");
  Serial.println(UID);
  
  bool result = checkUid(UID); 
  if (result) {
    client.add("auth_access", 1);  
    client.ubidotsPublish("esp32-project");
    showMessage("Access Granted"); 
    pushLogs(UID, "Access Granted"); 
    digitalWrite(BUZZER, LOW); 
    digitalWrite(RED_LED, LOW); 
    digitalWrite(BLUE_LED, HIGH); 
  } else {
    digitalWrite(BUZZER, HIGH); 
    digitalWrite(RED_LED, HIGH); 
    digitalWrite(BLUE_LED, LOW); 
    showMessage("Access Denied"); 
    pushLogs(UID, "Access Denied"); 
  }
  
  Serial.println();
  mfrc522.PICC_HaltA();
}
