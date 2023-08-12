#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DFRobot_MAX17043.h>

#define trigPin D10
#define echoPin D11

//ssl certificaat voor de mqtt broker
const char *ca_cert =
  "-----BEGIN CERTIFICATE-----\n"
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc="
  "-----END CERTIFICATE-----\n";

//batterij module
DFRobot_MAX17043 FuelGauge;

//meting variabelen
long duration;
float distance;
float batteryPercentage;

//wifi verbinding
const char *ssid = "Home 2.4ghz";
const char *password = "Telenet12345.";

//mqtt verbinding
WiFiClientSecure espClient;
PubSubClient client(espClient);
const char *mqtt_broker = "f07b2dbeeae343528d0b9dad5a13aebf.s1.eu.hivemq.cloud";
const char *tankLevelTopic = "watersensor_main_tank";
const char *batteryLevelTopic = "battery_level";
const char *mqtt_username = "watersensor_publish";
const char *mqtt_password = "ZEEZRrrze4235";
const int mqtt_port = 8883;

void setup() {
  // baudrate snelheid
  Serial.begin(115200);

  FuelGauge.begin();

  connectWiFi();

  pinMode(trigPin, OUTPUT);  //trigPin als OUTPUT
  pinMode(echoPin, INPUT);   //echoPin als INPUT
}

void doMeasurement() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0344 / 2;

  batteryPercentage = FuelGauge.readPercentage();

  //testing
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Battery level: ");
  Serial.print(batteryPercentage);
  Serial.println(" %");
}

void connectWiFi() {
  WiFi.begin(ssid, password);

  Serial.println("\nConnecting to wifi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
}

void sendMeasurement() {
  String sensorValue_str;
  String batteryLevel_str;
  char batteryLevel[50];
  char sensorValue[50];

  espClient.setCACert(ca_cert);
  client.setServer(mqtt_broker, mqtt_port);
  while (!client.connected()) {
    String client_id = "watersensor_publish";
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  sensorValue_str = String(distance);
  sensorValue_str.toCharArray(sensorValue, sensorValue_str.length() + 1);

  batteryLevel_str = String(batteryPercentage);
  batteryLevel_str.toCharArray(batteryLevel, batteryLevel_str.length() + 1);

  bool levelPublished = client.publish(tankLevelTopic, sensorValue);
  if(levelPublished == true) Serial.println("Water level published.");
  else Serial.println("Failed to publish water level.");

  bool batteryPublished = client.publish(batteryLevelTopic, batteryLevel);
  if(batteryPublished == true) Serial.println("Battery level published.");
  else Serial.println("Failed to publish battery level.");

  delay(5000);
}

void loop() {
  //wifi checken
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  //metingen
  doMeasurement();
  sendMeasurement();

  //deep sleep
  esp_sleep_enable_timer_wakeup(1800000000);
  Serial.println("Deep sleep enabled");
  esp_deep_sleep_start();
}