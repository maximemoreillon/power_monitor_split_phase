/*
 * POWER MONITOR SPLIT PHASE
 * Maxime MOREILLON
 * 
 * Board type: ESP32 DEv Board
 * 
 */

// Libraries
#include <WiFi.h>
#include <WiFiClient.h> 
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include "credentials.h";
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

#define HOSTNAME "power"

// Pin mapping
#define CT_1_PIN 36
#define CT_2_PIN 39

// MQTT
#define MQTT_BROKER_ADDRESS IPAddress(192, 168, 1, 2)
#define MQTT_PORT 1883
#define MQTT_STATUS_TOPIC "power/satus"
#define MQTT_LAST_WILL "{'state':'disconnected'}"
#define MQTT_QOS 1
#define MQTT_RETAIN true

// WIFI
#define WIFI_CONNECTION_TIMEOUT 5000

// Web server
#define WWW_PORT 80
#define WS_PORT 8087

// MQTT
#define MQTT_BROKER_ADDRESS IPAddress(192, 168, 1, 2)
#define MQTT_PORT 1883
#define MQTT_STATUS_TOPIC "power/status"
#define MQTT_LAST_WILL "{'state':'disconnected'}"
#define MQTT_QOS 1
#define MQTT_RETAIN true

// ADC reading
#define SAMPLE_COUNT 2000.00
#define FILTER_CONSTANT 0.001
#define VCC 3.30
#define ADC_MAX 4095.00
#define BURDEN_VALUE 22.00
#define CT_RATIO 2000.00

#define WS_SEND_PERIOD 100

#define MQTT_PUBLISH_PERIOD 10000

WiFiClient wifi_client;
PubSubClient MQTT_client(wifi_client);
WebServer web_server(WWW_PORT);
WebSocketsServer ws_server = WebSocketsServer(WS_PORT);


float RMS_current_phase_1;
float RMS_current_phase_2;

void setup() {
  // Mandatory initial delay
  delay(10);

  // Serial init
  Serial.begin(115200);
  Serial.println(); // Separate serial stream from initial gibberish
  Serial.println(F(__FILE__ " " __DATE__ " " __TIME__));
  
  wifi_setup();
  MQTT_setup();
  ota_setup();
  web_server_setup();
  websocket_setup();
}

void loop() {
  wifi_connection_manager();
  MQTT_connection_manager();
  ArduinoOTA.handle();
  web_server.handleClient();
  ws_server.loop();
  read_current();
  periodic_MQTT_publish();
  
}
