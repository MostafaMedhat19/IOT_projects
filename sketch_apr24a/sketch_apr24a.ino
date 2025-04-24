#include <WiFi.h>
#include <Firebase.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"
#define FIREBASE_HOST "https://rfiddooraccess-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyDnSBL8CRb6wsi2Ier5voQ4oFqv6sQ2guU"


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// RFID & hardware pins
#define GREEN_LED 4
#define RED_LED 5
#define BUZZER 2
#define RST_PIN 9
#define SS_PIN 10

String UID = "";
MFRC522 mfrc522(SS_PIN, RST_PIN);
Firebase firebase(FIREBASE_HOST, FIREBASE_AUTH);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // OLED Setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
  delay(2000);

  // WiFi Setup
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  // RFID Setup
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  UID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    UID += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println("Card UID: " + UID);

  bool result = checkUid(UID);

  display.clearDisplay();
  display.setCursor(0, 0);

  if (result) {
    Serial.println("Access Granted");
    display.println("Access Granted");
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
  } else {
    Serial.println("Access Denied");
    display.println("Access Denied");
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
    pushUid(UID);
  }

  display.display();
  delay(2000);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
}

bool checkUid(String id) {
  String path = "/authorized_users/" + id;
  String value = firebase.getString(path);
 
  
  Serial.println("Firebase Value: " + value);

  return value == "true";
}


void pushUid(String id) {
  String path = "/unauthorized_attempts/" + id;
int result = firebase.setString(path, "false");
if (result == 200) {
  Serial.println("Success!");
} else {
  Serial.println("Error code: " + String(result));
}
  Serial.println("Unauthorized UID pushed.");
}
