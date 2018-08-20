/**************************
 * Project pir_sensor
 * Description: This is a motion sensor publishing to an MQTT channel on a raspberry pi.
 * Author: Nadine Lessio
 * Date: July 10th 2018

 * resources / guides
 https://github.com/hirotakaster/MQTT
 https://community.particle.io/t/submission-mqtt-library-and-sample/2111/45
 https://learn.adafruit.com/pir-passive-infrared-proximity-motion-sensor/using-a-pir-w-arduino
 **************************/

 #include "elapsedMillis/elapsedMillis.h"
 #include "MQTT/MQTT.h"


// setup MQTT library and server
void callback(char* topic, byte* payload, unsigned int length);
byte server[] = { 192,168,0,3 }; //192.168.1.xx is ip of rpi mosquitto broker
MQTT client(server, 1883, callback);

// setup Pir Sensor pin and LED.
int ledPin = 7;
int pirPin = 2;
int pirState = LOW;
int val = 0;

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
}//callback

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  client.connect("particle_photon");
  client.publish("acelab/sensors/motion", "connected");
  Serial.begin(9600);
} // setup

void loop(){
  // read input value
  val = digitalRead(pirPin);
  // check if the input is HIGH
  if (val == HIGH) {
    // turn LED ON
    digitalWrite(ledPin, HIGH);
    if (pirState == LOW) {
      // we have just turned on
      // We only want to print on the output change, not state
      Serial.println("Motion detected!");
      client.publish("acelab/sensors/motion","on");
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW); // turn LED OFF
    if (pirState == HIGH){
      // we have just turned off
      // We only want to print on the output change, not state
      Serial.println("Motion ended!");
      client.publish("acelab/sensors/motion","off");
      pirState = LOW;
    }
  }

  if (!client.loop()) {
    client.connect("particle_photon");
    client.publish("room/motion","reconnect");
  }
} // loop
