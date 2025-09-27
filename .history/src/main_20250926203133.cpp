#include <Arduino.h>

// === Pino do LDR ===
// Se seu D5 mapear para GPIO14 (com ADC2), use 14.
// Se D5 na sua placa for GPIO18 (sem ADC), troque para 36 (A0) ou 34/35/32.
#define LDR_PIN 34  // D5 -> GPIO14 (ADC2_CH6) em muitos DevKit

// Suavização e limites (0..4095)
const int LDR_SAMPLES = 10;
int THRESH_DARK       = 10;
int THRESH_DIM        = 1200;
int THRESH_BRIGHT     = 2400;
int THRESH_VERYBRIGHT = 3400;

static int readLdrAvg() {
  long sum = 0;
  for (int i = 0; i < LDR_SAMPLES; i++) {
    sum += analogRead(LDR_PIN);
    delay(2);
  }
  return (int)(sum / LDR_SAMPLES);
}

void setup() {
  Serial.begin(115200);

  // Resolução do ADC (0..4095) e atenuação ~0..3.3V
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  Serial.println("\n--- LDR no ESP32 (porta D5) ---");
  Serial.print("LDR_PIN = ");
  Serial.println(LDR_PIN);
  Serial.println("Obs: GPIO14 usa ADC2 e pode conflitar com Wi-Fi.");
}

void loop() {
  int analogValue = readLdrAvg();

  Serial.print("Valor analogico = ");
  Serial.print(analogValue);
  Serial.print("  => ");

  if (analogValue < THRESH_DARK) {
    Serial.println("Escuro");
  } else if (analogValue < THRESH_DIM) {
    Serial.println("Meia-luz");
  } else if (analogValue < THRESH_BRIGHT) {
    Serial.println("Claro");
  } else if (analogValue < THRESH_VERYBRIGHT) {
    Serial.println("Brilhante");
  } else {
    Serial.println("Muito brilhante");
  }

  delay(500);
}