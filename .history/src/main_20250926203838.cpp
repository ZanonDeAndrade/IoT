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
Serial.println("Ambiente claro → Luz DESLIGADA");
}

delay(500);
}
