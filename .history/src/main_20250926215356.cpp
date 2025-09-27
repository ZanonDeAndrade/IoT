#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ======== PINOS ========
#define LDR_PIN   34   // ADC1 (32/33/34/35). Use 34 como exemplo
#define PIR_PIN   27   // OUT do PIR (HC-SR501)
#define LED_PIN   21   // LED externo: anodo->GPIO21->resistor, catodo->GND (ativo HIGH)

// ======== AJUSTES ========
#define LDR_THRESHOLD 50     // abaixo = escuro (calibre via Serial)
#define HOLD_MS       0      // 0 = sem retenção; ex.: 15000 mantém LED aceso 15s após movimento

// ======== Wi-Fi / MQTT ========
// >>>>> PREENCHA AQUI <<<<<
const char* WIFI_SSID     = "SEU_SSID";
const char* WIFI_PASS     = "SUA_SENHA";
const char* MQTT_BROKER   = "192.168.1.100";  // IP do Mosquitto
const uint16_t MQTT_PORT  = 1883;
const char* MQTT_USER     = "";               // se não usar auth, deixe vazio
const char* MQTT_PASS     = "";

// Identidade do dispositivo
const char* DEVICE_ID     = "esp32_sala";

// Tópicos (você pode adaptar a hierarquia)
String t_base     = String("esp32/") + DEVICE_ID + "/";
String t_state    = t_base + "state";        // JSON com ldr/pir/led/dark
String t_ldr      = t_base + "ldr";
String t_pir      = t_base + "pir";
String t_led      = t_base + "led";
String t_status   = t_base + "status";       // online/offline (LWT)
String t_cmd_led  = t_base + "cmd/led";      // "ON"/"OFF"/"AUTO"

// ======== ESTADO ========
unsigned long lastMotionMs = 0;
bool ledOn = false;
bool lastLedOn = false;
bool manualMode = false;     // false = AUTO (lógica escuro+movimento); true = manual por MQTT
bool manualLed = false;      // valor quando em manual
unsigned long lastPub = 0;

// MQTT
WiFiClient espClient;
PubSubClient mqtt(espClient);

// ======== AUXILIARES ========
void setLed(bool on) {
  digitalWrite(LED_PIN, on ? HIGH : LOW); // LED ativo HIGH
  ledOn = on;
}

void publishQuick(const String& topic, const String& payload, bool retain=false) {
  mqtt.publish(topic.c_str(), payload.c_str(), retain);
}

void publishStateJSON(int ldr, bool isDark, int pir) {
  // JSON simples
  char buf[160];
  snprintf(buf, sizeof(buf),
           "{\"ldr\":%d,\"dark\":%s,\"pir\":%d,\"led\":%s,\"mode\":\"%s\"}",
           ldr, isDark ? "true":"false", pir, ledOn ? "true":"false",
           manualMode ? "manual":"auto");
  publishQuick(t_state, String(buf), false);
}

// ======== MQTT CALLBACK ========
void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  String t = String(topic);
  String msg;
  msg.reserve(length+1);
  for (unsigned int i=0;i<length;i++) msg += (char)payload[i];
  msg.trim();
  msg.toUpperCase();

  if (t == t_cmd_led) {
    if (msg == "ON") {
      manualMode = true;
      manualLed = true;
      setLed(true);
      publishQuick(t_led, "ON", true);
    } else if (msg == "OFF") {
      manualMode = true;
      manualLed = false;
      setLed(false);
      publishQuick(t_led, "OFF", true);
    } else if (msg == "AUTO") {
      manualMode = false;
      // ao voltar pro AUTO, o loop decide o LED
    } else {
      // comando inválido, ignore
    }
  }
}

// ======== CONEXÕES ========
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi conectando");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi OK: "); Serial.println(WiFi.localIP());
}

void connectMQTT() {
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(onMqttMessage);
  // LWT
  mqtt.connect(DEVICE_ID, MQTT_USER, MQTT_PASS, t_status.c_str(), 0, true, "offline");
  // pode demorar um pouco, tente até conectar
  while (!mqtt.connected()) {
    Serial.print("Conectando MQTT...");
    if (mqtt.connect(DEVICE_ID, MQTT_USER, MQTT_PASS, t_status.c_str(), 0, true, "offline")) {
      Serial.println("OK");
      break;
    } else {
      Serial.print("falha rc="); Serial.print(mqtt.state()); Serial.println(" retry em 2s");
      delay(2000);
    }
  }
  // Publica status online (retain)
  publishQuick(t_status, "online", true);
  // Subscreve comandos
  mqtt.subscribe(t_cmd_led.c_str());
  // Opcional: informe modo atual
  publishQuick(t_base + "mode", manualMode ? "manual":"auto", true);
}

// ======== SETUP ========
void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  setLed(false);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  connectWiFi();
  connectMQTT();

  Serial.println("ESP32: LDR + PIR + LED + MQTT");
}

// ======== LOOP ========
void loop() {
  // mantém conexões
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();

  // ----- Leitura sensores -----
  int ldr = analogRead(LDR_PIN);       // 0..4095
  bool isDark = (ldr < LDR_THRESHOLD);

  int pir = digitalRead(PIR_PIN);
  if (pir == HIGH) lastMotionMs = millis();

  bool hold = (HOLD_MS > 0) ? (millis() - lastMotionMs < HOLD_MS) : (pir == HIGH);

  // ----- Lógica do LED -----
  if (manualMode) {
    setLed(manualLed);
  } else {
    bool shouldOn = isDark && hold;
    setLed(shouldOn);
  }

  // ----- Publicações -----
  // publica mudanças imediatas do LED (retain)
  if (ledOn != lastLedOn) {
    publishQuick(t_led, ledOn ? "ON" : "OFF", true);
    lastLedOn = ledOn;
  }

  // publica periodicamente estado e sensores
  if (millis() - lastPub > 5000) { // a cada 5s
    publishQuick(t_ldr, String(ldr), false);
    publishQuick(t_pir, pir == HIGH ? "1" : "0", false);
    publishStateJSON(ldr, isDark, pir);
    lastPub = millis();
  }

  // Debug opcional no Serial
  static unsigned long t0 = 0;
  if (millis() - t0 > 1000) {
    t0 = millis();
    Serial.print("LDR="); Serial.print(ldr);
    Serial.print(isDark ? " (escuro)" : " (claro)");
    Serial.print("  PIR="); Serial.print(pir);
    Serial.print("  LED="); Serial.print(ledOn ? "ON" : "OFF");
    Serial.print("  MODE="); Serial.println(manualMode ? "MANUAL" : "AUTO");
  }

  delay(10);
}