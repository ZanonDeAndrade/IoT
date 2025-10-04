#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>     // Para o Cloudflare D1
#include <ArduinoJson.h>    // Para o D1 e para o JSON do MQTT
#include <PubSubClient.h>   // Para o MQTT

// ===================================================
// --- PREENCHA TODAS AS SUAS INFORMAÇÕES AQUI ---
// ===================================================

// --- Wi-Fi ---
const char* WIFI_SSID = "AMF-CORP";
const char* WIFI_PASS = "SUA_SENHA_WIFI";

// --- Cloudflare Worker (para o Banco de Dados) ---
const char* WORKER_URL = "https://projeto-g1.beckerherica.workers.dev/insert"; 

// --- MQTT Broker ---
const char* MQTT_BROKER   = "test.mosquitto.org"; // Broker público para testes
const uint16_t MQTT_PORT  = 1883;
const char* MQTT_USER     = ""; // Deixe em branco se não precisar
const char* MQTT_PASS     = ""; // Deixe em branco se não precisar

// --- Identidade do Dispositivo ---
const char* DEVICE_ID = "esp32_sala"; // ID único para este dispositivo

// ===================================================
// --- PINOS E AJUSTES ---
// ===================================================
#define LDR_PIN   34
#define PIR_PIN   27
#define LED_PIN   21
#define LDR_THRESHOLD 50 // Limiar para considerar "escuro". Calibre conforme necessário.

// --- Controle de Tempo ---
unsigned long lastMqttPub = 0;
const long intervalMqttPub = 5000; // Publicar no MQTT a cada 5 segundos

unsigned long lastDbSend = 0;
const long intervalDbSend = 30000; // Enviar para o DB a cada 30 segundos

// ===================================================
// --- VARIÁVEIS DE ESTADO E OBJETOS GLOBAIS ---
// ===================================================

// Estado do dispositivo
bool ledOn = false;
bool lastLedOn = false;
bool manualMode = false;   // true = controlado por MQTT; false = automático
bool manualLed = false;    // Estado do LED quando em modo manual

// Tópicos MQTT (gerados dinamicamente)
String t_base    = String("esp32/") + DEVICE_ID + "/";
String t_state   = t_base + "state";      // JSON com estado completo
String t_led     = t_base + "led/state";  // Estado atual do LED (ON/OFF)
String t_status  = t_base + "status";      // online/offline (LWT)
String t_cmd_led = t_base + "led/command";  // Tópico para receber comandos

// Objetos de cliente
WiFiClient espClient;
PubSubClient mqtt(espClient);

// ===================================================
// --- FUNÇÕES AUXILIARES ---
// ===================================================

// Função para controlar o LED
void setLed(bool on) {
    digitalWrite(LED_PIN, on ? HIGH : LOW);
    ledOn = on;
}

// Função para publicar no MQTT
void publishMqtt(const String& topic, const String& payload, bool retain = false) {
    if (mqtt.connected()) {
        mqtt.publish(topic.c_str(), payload.c_str(), retain);
    }
}

// Função para enviar os dados para o banco na Cloudflare
void sendDataToCloudflare(int ldr, bool pir, bool led) {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(WORKER_URL);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> doc;
    doc["device_id"] = DEVICE_ID;
    doc["ldr"] = ldr;
    doc["movimento"] = pir ? 1 : 0;
    doc["led"] = led ? 1 : 0;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode > 0) {
        Serial.printf("[HTTP] Resposta do DB: %d\n", httpResponseCode);
    } else {
        Serial.printf("[HTTP] Erro no envio para o DB: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
}

// Callback: Função chamada quando uma mensagem MQTT é recebida
void onMqttMessage(char* topic, byte* payload, unsigned int length) {
    String t = String(topic);
    String msg;
    msg.reserve(length + 1);
    for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
    
    Serial.printf("[MQTT] Mensagem recebida no tópico: %s -> %s\n", topic, msg.c_str());

    if (t == t_cmd_led) {
        msg.toUpperCase();
        if (msg == "ON") {
            manualMode = true;
            manualLed = true;
            setLed(true);
        } else if (msg == "OFF") {
            manualMode = true;
            manualLed = false;
            setLed(false);
        } else if (msg == "AUTO") {
            manualMode = false;
            // A lógica principal no loop() vai assumir o controle
        }
    }
}

// Função para conectar/reconectar ao MQTT
void connectMQTT() {
    while (!mqtt.connected()) {
        Serial.print("[MQTT] Conectando ao broker...");
        if (mqtt.connect(DEVICE_ID, MQTT_USER, MQTT_PASS, t_status.c_str(), 1, true, "offline")) {
            Serial.println(" conectado!");
            publishMqtt(t_status, "online", true);
            mqtt.subscribe(t_cmd_led.c_str()); // Subscreve ao tópico de comando
        } else {
            Serial.printf(" falhou, rc=%d. Tentando novamente em 5s\n", mqtt.state());
            delay(5000);
        }
    }
}

// ===================================================
// --- SETUP ---
// ===================================================
void setup() {
    Serial.begin(115200);
    pinMode(PIR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    setLed(false);

    // Conecta ao Wi-Fi
    Serial.print("[WiFi] Conectando a ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[WiFi] Conectado!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());

    // Configura o cliente MQTT
    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setCallback(onMqttMessage);
}

// ===================================================
// --- LOOP ---
// ===================================================
void loop() {
    // 1. Manutenção das conexões
    if (WiFi.status() != WL_CONNECTED) {
        // Tenta reconectar o WiFi
    }
    if (!mqtt.connected()) {
        connectMQTT();
    }
    mqtt.loop(); // Essencial para o MQTT funcionar (processar mensagens recebidas)

    // 2. Leitura dos Sensores
    int ldrValue = analogRead(LDR_PIN);
    bool isDark = (ldrValue < LDR_THRESHOLD);
    bool motionDetected = (digitalRead(PIR_PIN) == HIGH);

    // 3. Lógica de Controle do LED
    if (manualMode) {
        // Em modo manual, o LED é controlado pela função de callback do MQTT
        setLed(manualLed);
    } else {
        // Em modo automático, a lógica é escuridão + movimento
        bool shouldBeOn = isDark && motionDetected;
        setLed(shouldBeOn);
    }

    // 4. Publicação de Dados (em intervalos de tempo)
    unsigned long now = millis();

    // Publica no MQTT
    if (now - lastMqttPub > intervalMqttPub) {
        lastMqttPub = now;
        Serial.println("[MQTT] Publicando status...");
        
        // Publica o estado do LED (se mudou)
        if (ledOn != lastLedOn) {
            publishMqtt(t_led, ledOn ? "ON" : "OFF", true);
            lastLedOn = ledOn;
        }

        // Cria e publica o JSON de estado completo
        StaticJsonDocument<200> doc;
        doc["ldr"] = ldrValue;
        doc["movimento"] = motionDetected;
        doc["led"] = ledOn;
        doc["modo"] = manualMode ? "manual" : "auto";
        String jsonState;
        serializeJson(doc, jsonState);
        publishMqtt(t_state, jsonState);
    }

    // Envia para o Banco de Dados
    if (now - lastDbSend > intervalDbSend) {
        lastDbSend = now;
        Serial.println("[HTTP] Enviando para o banco de dados...");
        sendDataToCloudflare(ldrValue, motionDetected, ledOn);
    }
    
    delay(10); // Pequeno delay para estabilidade
}