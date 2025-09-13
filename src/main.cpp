#include <Arduino.h>

/* 
 * Projeto: Sensor de luz com ESP32
 * Fonte original: esp32io.com
 * Código em domínio público
 * 
 * Funcionalidade:
 * - Se o ambiente estiver escuro -> LED acende
 * - Se houver luz -> LED apaga
 */

#define LIGHT_SENSOR_PIN 4   // D2
#define LED_PIN 22           // D22             
#define ANALOG_THRESHOLD 0 // ajuste conforme seu ambiente

void setup() {
  // Configuração do ADC (0 a 3.3V)
  analogSetAttenuation(ADC_11db);

  pinMode(LED_PIN, OUTPUT);

  // Inicializa o monitor serial
  Serial.begin(115200);
  Serial.println("Iniciando sensor de luz...");
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
    digitalWrite(LED_PIN, HIGH);   // Claro -> apaga LED
}


  delay(500); // atraso de meio segundo
}
