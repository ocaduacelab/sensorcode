/************************ 
Photocell Sensor
ACE Lab July 2018
v1.0
Nadine Lessio

Basic photocell sensor that will subscribe to an MQTT channel on the Raspberry Pi in the room.

Note: The ADC pin on the feather huzzah will only take 1v. So you will have to play w/ different resistors to get a reading and not fry your board!
Usually 2.2 Kohm resistor will work well. 
*************************/
 
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "acelabwifi"
#define WLAN_PASS       "ace!lab12#"

/************************* Setup *********************************/
#define SERVER      "192.168.0.3"
#define SERVERPORT  1883
#define ROOM_LIGHT "acelab/sensors/brightness"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT);
Adafruit_MQTT_Publish feedlight = Adafruit_MQTT_Publish(&mqtt, ROOM_LIGHT);


#define PHOTOCELL_PIN A0
int current = 0;
int last = -1;

void setup() {
   // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: ")); Serial.println(WiFi.localIP());
  Serial.begin(115200);
} // setup

void loop() {
  MQTT_connect();

	current = analogRead(PHOTOCELL_PIN);
	if(current == last)
		return;

	// save the current state to the analog feed
  Serial.print("printing -> ");
  Serial.println(current);

  last = current;

  if (! feedlight.publish(current)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK! Light."));
  }
  // wait 2 minutes
  delay(120000);
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
