#include <Arduino.h>

// Ajuste aqui para o pino analógico que sua placa realmente tem:
// Se for ESP32 → use GPIO34, 35, 32, 33 (ADC1).
// Se for ESP8266 NodeMCU → use A0.
#define LDR_PIN 34   // exemplo: GPIO34

// Defina o limite (0..4095). Abaixo = escuro, acima = claro.
const int LDR_THRESHOLD = 50;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);       // só no ESP32
  analogSetAttenuation(ADC_11db); // só no ESP32
  Serial.println("Sensor LDR iniciado...");
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);

  if (ldrValue < LDR_THRESHOLD) {
    Serial.println("Luz DESLIGADA (escuro)");
  } else {
    Serial.println("Luz LIGADA (claro)");
  }

  delay(500);
}