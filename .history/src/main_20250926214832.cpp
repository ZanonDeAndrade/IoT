#include <Arduino.h>

// === PINOS ===
#define LDR_PIN   34   // use um ADC1 real: 32, 33, 34 ou 35 (nó do divisor)
#define PIR_PIN   2  // OUT do PIR (HC-SR501)
#define LED_PIN   21   // LED externo (anodo->GPIO21->resistor, catodo->GND)

// === AJUSTES ===
#define LDR_THRESHOLD 50   // abaixo disso = escuro (calibre vendo no Serial)
#define HOLD_MS       0     // 0 = sem retenção; ex.: 15000 mantém LED 15s após movimento

// === ESTADO ===
unsigned long lastMotionMs = 0;

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);     // PIR entrega HIGH no movimento
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // LED começa apagado (ativo HIGH)

  analogReadResolution(12);       // 0..4095
  analogSetAttenuation(ADC_11db); // até ~3.3V

  Serial.println("ESP32: LDR + PIR + LED (LED ativo HIGH)");
}

void loop() {
  // ----- LDR -----
  int ldr = analogRead(LDR_PIN);      // nó do divisor (LDR-3V3 / 10k-GND)
  bool isDark = (ldr < LDR_THRESHOLD);

  // ----- PIR -----
  int pir = digitalRead(PIR_PIN);
  if (pir == HIGH) lastMotionMs = millis();

  bool hold = (HOLD_MS > 0) ? (millis() - lastMotionMs < HOLD_MS) : (pir == HIGH);

  // ----- LÓGICA DO LED -----
  bool ledOn = isDark && hold;        // LED só liga se escuro + movimento/hold
  digitalWrite(LED_PIN, ledOn ? HIGH : LOW);   // LED ativo HIGH

  // Debug rápido para calibrar
  static unsigned long t0 = 0;
  if (millis() - t0 > 400) {
    t0 = millis();
    Serial.print("LDR="); Serial.print(ldr);
    Serial.print(isDark ? " (escuro)" : " (claro)");
    Serial.print("  PIR="); Serial.print(pir);
    Serial.print("  LED="); Serial.println(ledOn ? "ON" : "OFF");
  }

  delay(10);
}