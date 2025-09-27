#include <Arduino.h>
#define LED_PIN 21   // D21 / GPIO21

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Teste LED no GPIO21: fonte e dreno");
}

void loop() {
  // Modo "fonte" (LED anodo no pino, catodo no GND, com resistor em série)
  Serial.println("Fonte: HIGH (deveria acender se anodo->pino, catodo->GND)");
  digitalWrite(LED_PIN, HIGH);
  delay(2000);

  Serial.println("Fonte: LOW (apaga)");
  digitalWrite(LED_PIN, LOW);
  delay(1000);

  // Modo "dreno" (LED anodo no 3V3, catodo no pino, com resistor em série)
  Serial.println("Dreno: LOW (deveria acender se anodo->3V3, catodo->pino)");
  digitalWrite(LED_PIN, LOW);
  delay(2000);

  Serial.println("Dreno: HIGH (apaga)");
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
}