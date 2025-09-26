const int PIR_PIN = 4;        // Pino do PIR
const int LED_PIN = 16;       // Pino do LED
const int LDR_PIN = 34;       // Pino do LDR
const int LIMIAR_LUZ = 2000;  // Limiar de luz (ajustável)


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