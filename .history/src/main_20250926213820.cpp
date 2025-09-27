#include <Arduino.h>
#define LED_PIN 21

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);  // LED ligado
  delay(1000);
  digitalWrite(LED_PIN, LOW);   // LED desligado
  delay(1000);
}