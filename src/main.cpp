#include <ESP8266WiFi.h>
#include <PubSubClient.h>

char ssid[] = "ESGI";
char pass[] = "Reseau-GES";

int status = WL_IDLE_STATUS;

WiFiClient client;
IPAddress server(146,59,147,99);  // Jeedom

#define LED_PIN_BOARD D1
#define SENSOR_PIN_ANAL A0

// char user[] = "****";
// char pass[] = "****";

bool STATE = false;

void open();
void close();

String payloadToString(byte *payload, unsigned int length)
{
  char buffer[length];
  sprintf(buffer, "%.*s", length, payload);
  return String(buffer);
}

String getLightLevel () 
{
  // delay(5000);
  int16_t lightLevel = (1023-analogRead(SENSOR_PIN_ANAL))/10.23;
  return String(lightLevel);
}


void callback(char *topic, byte *payload, unsigned int length)
{
  String message = payloadToString(payload, length);
  int value = message.toInt();
  if (message == "on") {
    Serial.println(topic);
    digitalWrite(LED_PIN_BOARD, HIGH);
  }else if (message == "off") {
    digitalWrite(LED_PIN_BOARD, LOW);
  }
}

void setup () 
{
  Serial.begin(9600);
  pinMode(LED_PIN_BOARD, OUTPUT);
  delay(1000);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");

  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Can't connect");
    
      delay(500);
  }

  Serial.println("Connected to wifi");
  Serial.println("\nStarting connection...");
}

PubSubClient mqtt(server, 1883, callback, client);

void reconnect()
{
  while (!mqtt.connected())
  {
    Serial.println("[MQTT] Connecting to MQTT...");
    if (mqtt.connect("TempCapteur", "jeedom", "jeedom"))
    {
      Serial.println("[MQTT] Connected");

      Serial.println("[MQTT] Message sent");
      // mqtt.subscribe("light/sensor");
      mqtt.subscribe("shutter/action");

      Serial.println("[MQTT] Message subscribed");
    }
    else
    {
      Serial.print("[ERROR] MQTT Connect: ");
      // Serial.println(mqtt.state());
      delay(60 * 1000);
    }
  }
}

/*void open() {
  Serial.println("Open shutter");
  digitalWrite(LED_PIN_BOARD, HIGH);
  delay(5000);
  digitalWrite(LED_PIN_BOARD, LOW);
  STATE = true;
  mqtt.publish("shutter/status", String(STATE).c_str());
}

void close() {
  Serial.println("Close shutter");
  digitalWrite(LED_PIN_BOARD, HIGH);
  delay(5000);
  digitalWrite(LED_PIN_BOARD, LOW);
  STATE = false;
  mqtt.publish("shutter/status", String(STATE).c_str());
}*/

void loop() 
{
  if (!mqtt.connected())
  {
    reconnect();
  }
  mqtt.publish("light/sensor", getLightLevel().c_str());
  mqtt.loop();
  
  Serial.println(getLightLevel().c_str());
 
  
}
