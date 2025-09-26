#include <Arduino.h>

#define PIR_PIN GPIO_NUM_D4     // Pino digital do PIR
#define LED_PIN GPIO_NUM_16    // Pino do LED
#define LDR_PIN 34             // Pino analógico do LDR (sensor de luz)

// Defina um limiar de luminosidade (ajuste com testes)
#define LIMIAR_LUZ 2000        

void setup() {
  Serial.begin(115200);

  pinMode((int)PIR_PIN, INPUT);
  pinMode((int)LED_PIN, OUTPUT);

  // Configura wake-up externo pelo PIR (nível alto)
  esp_sleep_enable_ext0_wakeup(PIR_PIN, 1);  

  Serial.println("ESP32 iniciado, aguardando movimento...");
}

void loop() {
  int movimento = digitalRead((int)PIR_PIN);   // Lê PIR
  int valorLuz = analogRead(LDR_PIN);          // Lê LDR

  Serial.print("Movimento: ");
  Serial.print(movimento);
  Serial.print(" | Luz: ");
  Serial.println(valorLuz);

  // Condição: só acende se houver movimento e estiver escuro
  if (movimento == HIGH && valorLuz < LIMIAR_LUZ) {
    digitalWrite((int)LED_PIN, HIGH);
    Serial.println("💡 Movimento detectado e ambiente escuro -> LED LIGADO");
    delay(5000);  // Mantém LED ligado por 5 segundos
    digitalWrite((int)LED_PIN, LOW);
    Serial.println("LED apagado");
  }

  // Vai para deep sleep até próximo movimento
  Serial.println("Indo para deep sleep...");
  esp_deep_sleep_start();  
}
