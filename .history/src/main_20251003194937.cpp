#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // Essencial para criar o JSON

// --- SUAS CONFIGURAÇÕES ---
const char* WIFI_SSID = "AMF-CORP";
const char* WIFI_PASS = "@MF$4515";
// URL do Worker que você publicou (com a rota /insert no final)
const char* WORKER_URL = ""; 
const char* DEVICE_ID = "esp32_sala"; // Identificador único do seu dispositivo

// --- PINOS DOS SENSORES (adapte aos seus) ---
const int LDR_PIN = 34;
const int PIR_PIN = 27;
const int LED_PIN = 21;

// --- VARIÁVEIS DE CONTROLE ---
unsigned long ultimoEnvioAPI = 0;
const long intervaloEnvioAPI = 30000; // Enviar dados a cada 30 segundos

// Função para enviar os dados para a API na Cloudflare
void enviarDadosParaAPI(int valorLDR, bool movimento, bool ledAceso) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ WiFi desconectado. Envio cancelado.");
        return;
    }

    Serial.println("📡 Preparando para enviar dados para a API...");

    // Cria um documento JSON com capacidade para os nossos dados
    StaticJsonDocument<200> doc;
    doc["device_id"] = DEVICE_ID;
    doc["ldr"] = valorLDR;
    doc["movimento"] = movimento ? 1 : 0; // Converte booleano para 1 ou 0
    doc["led"] = ledAceso ? 1 : 0;      // Converte booleano para 1 ou 0

    String payload;
    serializeJson(doc, payload); // Converte o documento JSON para uma String

    HTTPClient http;
    http.begin(WORKER_URL);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(payload);

    if (httpCode > 0) {
        String response = http.getString();
        Serial.print("✅ Resposta da API (HTTP ");
        Serial.print(httpCode);
        Serial.println("):");
        Serial.println(response);
    } else {
        Serial.print("❌ Erro no envio HTTP: ");
        Serial.println(http.errorToString(httpCode));
    }

    http.end();
}

void setup() {
    Serial.begin(115200);
    pinMode(PIR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Conecta ao WiFi
    Serial.print("Conectando ao WiFi ");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n📶 WiFi Conectado!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // A lógica do seu projeto de acender e apagar o LED continua aqui...
    // Por exemplo:
    int valorLDR = analogRead(LDR_PIN);
    bool temMovimento = digitalRead(PIR_PIN) == HIGH;
    bool ledEstaAceso = digitalRead(LED_PIN) == HIGH; // Lê o estado atual do pino do LED

    // Exemplo simples de lógica:
    if (temMovimento && valorLDR < 1000) { // Limiar de exemplo para "escuro"
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }
    
    // --- LÓGICA DE ENVIO PARA A API ---
    // Verifica se já passou o tempo para um novo envio
    if (millis() - ultimoEnvioAPI > intervaloEnvioAPI) {
        ultimoEnvioAPI = millis(); // Marca o tempo do envio atual
        
        // Chama a função para enviar os dados mais recentes dos sensores
        enviarDadosParaAPI(valorLDR, temMovimento, ledEstaAceso);
    }

    delay(100); // Pequeno delay para estabilidade
}