#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define mqtt_port 1883
/*
const char* mqtt_server = "odilon.ddns.net";
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_server = "test.mosquitto.org";
*/
const char* mqtt_server = "iot.eclipse.org";

String robuinoName;

WiFiClient espClient;
PubSubClient MQTTclient(espClient);

#define ROBUINO_MAIN_TOPIC "robuino/"
#define CONNECTION_SUBTOPIC "connection/"
#define ACTION_SUBTOPIC "action/"
#define RETCODE_SUBTOPIC "retcode/"
#define AUTOTASK_SUBTOPIC "autotask/"
#define ALL_ROBUINO "all"
#define CONNECTION_MESSAGE "connected now..."

void initMQTT() {
   MQTTclient.setServer(mqtt_server, mqtt_port);
   MQTTclient.setCallback(callbackMQTT);
}

/*
 * MQTT callback() function 
 */
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
   Serial.print("Message arrived [");
   Serial.print(topic);
   Serial.println("]");
   Serial.println((const char *)payload);

   payload[length] = '\0';   //força ultimo byte null - estava mantendo caracter anterior

   PublishMsgMQTT ( (String)topic, "resposta from arduino");
}

void PublishMsgMQTT (String stringTopic, String stringMsg) {
   char topic[stringTopic.length()+1];
   strcpy(topic, stringTopic.c_str());
   char msg[stringMsg.length()+1];
   strcpy(msg, stringMsg.c_str());
   MQTTclient.publish(topic, msg);
}

//reconnect to MQTT broker
void reconnectMQTT() {
   // Loop until we're reconnected
   while (!MQTTclient.connected()) {
      robuinoName = "esp8266-12e";
      robuinoName.toLowerCase();
      Serial.print("Attempting MQTT connection to ");Serial.print(robuinoName);
      if (MQTTclient.connect(robuinoName.c_str())) {
         Serial.print(" "); Serial.println(CONNECTION_MESSAGE);
         // Once connected, publish an announcement...
         String mqttTopic = ROBUINO_MAIN_TOPIC;
         String mqttPublishTopic = mqttTopic + CONNECTION_SUBTOPIC + robuinoName;
/*
 * the Robuino client once connected publish an announcement
 * to the topic: robuino/connection/robuinoName
 * with the message CONNECTION_MESSAGE
 */
         MQTTclient.publish(mqttPublishTopic.c_str(), CONNECTION_MESSAGE);
/*
 * and then the Robuino client will subscribe to:
 * robuino/action/# ---> robuino/action/robuinoName/operator
 */
         // ... and resubscribe
         String mqttSubscribeTopic = mqttTopic + ACTION_SUBTOPIC + "#";
         MQTTclient.subscribe(mqttSubscribeTopic.c_str());
      } else {
         Serial.print(" failed, rc=");
         Serial.print(MQTTclient.state());
         Serial.println(" try again in 5 seconds");
         // Wait 5 seconds before retrying
         delay(5000);
      }
   }
}

void loopMQTT(){
   if (!MQTTclient.connected()) {
      reconnectMQTT();
   }
   MQTTclient.loop();
}


