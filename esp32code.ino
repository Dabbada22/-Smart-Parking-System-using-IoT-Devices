#include <WiFi.h>
#include "ThingSpeak.h"

// WiFi credentials
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// ThingSpeak credentials
unsigned long myChannelNumber = YOUR_CHANNEL_NUMBER;
const char *myWriteAPIKey = "YOUR_WRITE_API_KEY";

// Sensor pin definitions (same as Arduino code)
const int trigPin = 9;
const int echoPin = 10;
const int carPresentThreshold = 20; // in cm

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  
  ThingSpeak.begin(client);

  // Set up sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Sensor logic to get distance
  long duration;
  int distance;
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Determine parking status
  int slotStatus = (distance < carPresentThreshold) ? 1 : 0; // 1 for occupied, 0 for free

  // Update ThingSpeak channel
  ThingSpeak.setField(1, slotStatus);
  
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(20000); // Wait 20 seconds before the next update
}
