const int PIR_PIN 4;      // Pino do PIR (digital, GPIO4 é válido no ESP32)
const int LED_PIN 16     // LED no GPIO16
const int LDR_PIN 34     // LDR no GPIO34 (pino ADC)

// Ajuste conforme os valores que aparecerem no Serial Monitor
#define LIMIAR_LUZ 2000  

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.println("Iniciando teste de PIR + LDR...");
}

void loop() {
  int movimento = digitalRead(PIR_PIN);   // 0 ou 1
  int valorLuz = analogRead(LDR_PIN);     // 0 ~ 4095 no ESP32

  Serial.print("Movimento: ");
  Serial.print(movimento);
  Serial.print(" | Luz: ");
  Serial.println(valorLuz);

  // Condição: só acende LED se estiver escuro e houver movimento
  if (movimento == HIGH && valorLuz < LIMIAR_LUZ) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(500);
}