#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

/* 
 * Projeto: Sensor de luz com ESP32
 * Fonte original: esp32io.com
 * Código em domínio público
 *
 * Funcionalidade:
 * - Se o ambiente estiver escuro -> LED acende
 * - Se houver luz -> LED apaga
 * - Envia leituras via MQTT para broker online
 */

// ===== PINOS =====
#define LIGHT_SENSOR_PIN 4   // D2
#define LED_PIN 22           // D22
#define ANALOG_THRESHOLD 0   // ajuste conforme seu ambiente

// ===== CONFIGURAÇÕES WIFI =====
const char* ssid = "AMF-CORP";       // <<<<< coloque aqui seu WiFi
const char* password = "@MF$4515";  // <<<<< coloque aqui sua senha

// ===== CONFIGURAÇÕES MQTT =====
const char* mqtt_server = "test.mosquitto.org"; 
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/luz";  // tópico público

WiFiClient espClient;
PubSubClient client(espClient);

// ===== Função para reconectar ao MQTT =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP32Client12345")) {  // ID único do cliente
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void setup() {
  // Configuração do ADC (0 a 3.3V)
  analogSetAttenuation(ADC_11db);

  pinMode(LED_PIN, OUTPUT);

  // Inicializa o monitor serial
  Serial.begin(115200);
  Serial.println("Iniciando sensor de luz...");

  // Conectar ao WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  // Configurar servidor MQTT
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  // Leitura do LDR
  int analogValue = analogRead(LIGHT_SENSOR_PIN);
  Serial.print("Luminosidade: ");
  Serial.println(analogValue);

  // Controle do LED
  if (analogValue > ANALOG_THRESHOLD) {
    digitalWrite(LED_PIN, LOW);  // Escuro -> liga LED
  } else {
    digitalWrite(LED_PIN, HIGH); // Claro -> apaga LED
  }

  // ===== MQTT =====
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publicar valor da luminosidade no tópico MQTT
  char msg[50];
  sprintf(msg, "%d", analogValue);
  client.publish(mqtt_topic, msg);

  delay(500); // atraso de meio segundo
}
