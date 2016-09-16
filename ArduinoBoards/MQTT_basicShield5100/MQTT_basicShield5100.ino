/*
 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 199); //odi IPAddress ip(172, 16, 0, 100);
//odi IPAddress server(172, 16, 0, 2);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] length ");Serial.print(length); Serial.print(" ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClientodi")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("teste/outTopic","hello world");
      // ... and resubscribe
      client.subscribe("teste/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

const char* mqtt_server = "iot.eclipse.org"; //"broker.mqtt-dashboard.com";
#define SERVER "iot.eclipse.org" //odi
#define PORT 1883                //odi
#define UART_SERIAL 57600        //odi
void setup()
{
  Serial.begin(UART_SERIAL);

  client.setServer(mqtt_server, 1883); //odi client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
//odi
   IPAddress ipAdress = Ethernet.localIP();
   Serial.println(ipAdress);
//odi
  // Allow the hardware to sort itself out
  delay(1500);
}

long lastMsg = 0;
char msg[50];
long value = 0;
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("teste/outTopic", msg);
  }
}
