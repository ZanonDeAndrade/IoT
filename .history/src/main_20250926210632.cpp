#include <Arduino.h>

// --- CONFIGURAÇÃO DO LDR ---
#define LDR_PIN 34             // exemplo: GPIO34 no ESP32
const int LDR_THRESHOLD = 50;  // Ajuste: abaixo = escuro, acima = claro

// --- CONFIGURAÇÃO DO PIR ---
const int PIR_PIN = 2;         // GPIO ligado ao pino OUT do sensor PIR

void setup() {
Serial.begin(115200);

// Configuração LDR (apenas ESP32)
analogReadResolution(12);
analogSetAttenuation(ADC_11db);

// Configuração PIR
pinMode(PIR_PIN, INPUT);

Serial.println("Sistema de Sensor LDR + PIR iniciado...");
}

void loop() {
// Leitura do LDR
int ldrValue = analogRead(LDR_PIN);
bool escuro = (ldrValue < LDR_THRESHOLD);

// Leitura do PIR
int pirState = digitalRead(PIR_PIN);
bool movimento = (pirState == HIGH);

// Lógica de funcionamento:
if (escuro && movimento) {
Serial.println(">>> Movimento detectado no ESCURO → LUZ LIGADA");
} else if (escuro && !movimento) {
Serial.println("Escuro, mas sem movimento → Luz DESLIGADA");
} else {
Serial.println("Ambiente claro → Luz DESLIGADA");/*
 * Código para ESP32 - LDR + LED
 * Baseado em esp32io.com (domínio público)
 */

// Constantes
#define LIGHT_SENSOR_PIN 36   // GPIO36 (ADC0) -> ligado ao LDR
#define LED_PIN 21            // D21 / GPIO21 -> LED externo
#define ANALOG_THRESHOLD 500  // ajuste conforme seu teste (0..4095)

void setup() {
  // Configuração ADC
  analogSetAttenuation(ADC_11db);  // até ~3,3V de entrada

  // LED como saída
  pinMode(LED_PIN, OUTPUT);

  // (opcional) Monitor serial
  Serial.begin(115200);
}

void loop() {
  int analogValue = analogRead(LIGHT_SENSOR_PIN); // lê valor do LDR

  if (analogValue < ANALOG_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);  // Liga LED
    Serial.println("Escuro -> LED ON");
  } else {
    digitalWrite(LED_PIN, LOW);   // Desliga LED
    Serial.println("Claro -> LED OFF");
  }

  delay(500);
}
}

delay(500);
}
