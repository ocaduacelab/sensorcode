/************************ 
Temp / Humidity Sensor
ACE Lab July 2018
v1.0
Nadine Lessio

Basic temp / humidity sensor that publishes to an MQTT channel on a pi. 
*************************/

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "acelabwifi"
#define WLAN_PASS       "ace!lab12#"

/************************* Setup *********************************/
#define SERVER      "192.168.0.3"
#define SERVERPORT  1883
#define ROOM_TEMP "acelab/sensors/temp"
#define ROOM_HUMID "acelab/sensors/humid"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT);
Adafruit_MQTT_Publish feedtemp = Adafruit_MQTT_Publish(&mqtt, ROOM_TEMP);
Adafruit_MQTT_Publish feedhumidity = Adafruit_MQTT_Publish(&mqtt, ROOM_HUMID);

#define DHTPIN 2     
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);

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
  Serial.println("DHTxx test!");
  dht.begin();
  
}

void loop(){
  MQTT_connect();

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("p\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  if (! feedtemp.publish(t)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK! Temp."));
  }

  if (! feedhumidity.publish(hic)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK! Humid"));
  }

  // 2 minute delay
  // look at doing a sleep or deep sleep and wake up call. 
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
