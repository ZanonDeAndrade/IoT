/*
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