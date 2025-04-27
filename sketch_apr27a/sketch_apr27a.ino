#include <Firebase.h>
#include "UbidotsESPMQTT.h"
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RST_PIN   5    
#define SS_PIN    2    
#define TOKEN "BBUS-IIoL8c1xc5CGXMADdtBI3JPXhXUNK2"     
#define WIFINAME "Mostafa-Elashry" 
#define WIFIPASS "12345678"  
#define REFERENCE_URL "https://rfiddooraccess-default-rtdb.firebaseio.com/"
#define AUTH_TOKEN "6tFsRuLvPmfgFJMVQFF8UeTqTs3DS88WbDysDK8B"
#define LED_BLUE 12
#define LED_RED 14
#define BUZZER 27

MFRC522 mfrc522(SS_PIN, RST_PIN);   
Firebase fb(REFERENCE_URL, AUTH_TOKEN);
Ubidots client(TOKEN);
String UID = "";
int counter = 0; 
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void displayMessage(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(msg);
  display.display(); 
}

bool checkUid(String id) {
  String path = "users/" + id; 
  int value = fb.getInt(path);
  if (value == 1) {
    return true; 
  } else {
    counter++;
    return false; 
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BLUE, OUTPUT); 
  pinMode(LED_RED, OUTPUT); 
  pinMode(BUZZER, OUTPUT); 
  digitalWrite(LED_BLUE, LOW); 
  digitalWrite(LED_RED, LOW); 
  digitalWrite(BUZZER, LOW); 

  client.setDebug(true);  
  client.wifiConnection(WIFINAME, WIFIPASS);
  client.begin(callback);

  delay(1000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  Serial.println("OLED initialized");
  display.clearDisplay();
  display.display();

  Serial.print("Connecting to: ");
  Serial.println(WIFINAME);
  WiFi.begin(WIFINAME, WIFIPASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi Connected");
  SPI.begin();         // Initialize SPI bus
  mfrc522.PCD_Init();  // Initialize MFRC522 card
  Serial.println(F("Warning: this example overwrites the UID of your UID changeable card, use with care!"));
  
  displayMessage("Connected to WiFi!");
}

void pushLogs(String message, String id) {
  String path = "/users/logs";
  fb.setString(path, id); 
  fb.pushString("/users/logs/" + id, message);
}

void loop() {
  if (!client.connected()) {
    client.reconnect();
  }

  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }
  
  UID = ""; // Reset UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    UID += String(mfrc522.uid.uidByte[i]);
  } 

  Serial.print("Card UID: " + UID);
  Serial.println();
  
  bool result = checkUid(UID); 
  if (result) {
    displayMessage("Access Granted");
    pushLogs(UID, "Access Granted"); 
  } else {
    displayMessage("Access Denied");
    pushLogs(UID,"Access Denied" ); 
  }
 
  client.add("auth_access", counter);  
  client.ubidotsPublish("esp32-project");
}