#include <Arduino.h>

// Definições de pinos
#define PIR_PIN 4
#define LDR_PIN 34   // pino ADC para LDR
#define LED_PIN 18

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int movimento = digitalRead(PIR_PIN);
  int luz = analogRead(LDR_PIN);  // valor de 0 a 4095 no ESP32

  Serial.print("Movimento: ");
  Serial.print(movimento);
  Serial.print(" | Luz: ");
  Serial.println(luz);

  // Acende LED somente se houver movimento e ambiente estiver escuro
  // supondo que no escuro a leitura seja > 2500
  if (movimento == HIGH && luz > 2500) {
      digitalWrite(LED_PIN, HIGH);
  } else {
      digitalWrite(LED_PIN, LOW);
  }


  delay(200); // atraso pequeno para estabilidade
}
