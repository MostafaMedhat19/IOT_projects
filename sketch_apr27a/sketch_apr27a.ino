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

//-----------------------------------------
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "UbidotsESPMQTT.h"

#define REFERENCE_URL ""
#define AUTH_TOKEN ""
#define RST_PIN       
#define SS_PIN     
#define TOKEN "....."    
#define WIFI_SSID "....." 
#define WIFI_PASSWORD "....."

#define BUZZER 
#define BLUE_LED 
#define RED_LED 

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

Ubidots client(TOKEN);
MFRC522 mfrc522(SS_PIN, RST_PIN); 
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &WIRE , -1 );
Firebase fb(REFERENCE_URL, AUTH_TOKEN);
String UID   =""; 
int counter = 0 ; 
bool checkUid(String id)
{
   String path = "users/"+id; 
   int value = fb.getInt(path); 
   if(!value)
      {
        
        return false ;
      }
      counter++; 
    return true ; 
}
void showMessage(String msg)
{
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print(msg);
  display.setCursor(0,0);
  display.display(); 
}
void pushLogs(String id ,String message )
{
  
   fb.pushString("logs" , id + "  " + message );
}
void setup() {

  Serial.begin(115200);  
  while (!Serial);    
  SPI.begin();       
  mfrc522.PCD_Init(); 
  pinMode(BUZZER , OUTPUT); 
  pinMode(BLUE_LED, OUTPUT); 
  pinMode(RED_LED, OUTPUT); 
  digitalWrite(BUZZER , LOW); 
  digitalWrite(BLUE_LED, LOW);
   digitalWrite(RED_LED, LOW);
  Serial.println(F("Warning: this example overwrites the UID of your UID changeable card, use with care!"));
   display.display();
  delay(1000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 

  Serial.println("OLED begun");
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("connected!");
  display.println("IP: 10.0.1.23"); 
  display.setCursor(0,0);
  display.display(); 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("-");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println();
  Serial.begin(115200);
  client.setDebug(true);  
  client.wifiConnection(WIFI_SSID, WIFI_PASSWORD);
  client.begin(callback);
}

void loop() {
  if (!client.connected()) {
    client.reconnect();
  }
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  UID=""; 
  for (byte i = 0; i < mfrc522.uid.size; i++) {
       UID +=String(mfrc522.uid.uidByte[i]);
   
  } 
  Serial.print(F("Card UID:"+UID));
  bool result = checkUid(UID); 
  if(result)
     {
        client.add("stuff", 1);  
        client.add("stuff", UID.toInt());  
        client.ubidotsPublish("source1");
        showMessage("Acccess Granted"); 
        pushLogs(UID ,"Acccess Granted" ); 
     }
     else
     {
     
         showMessage("Acccess Denied"); 
          pushLogs(UID ,"Acccess Denied" ); 
     }
   
  Serial.println();
 
}
