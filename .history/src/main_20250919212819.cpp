// Definição dos pinos
const int PIR_PIN = 4;       // PIR no GPIO4
const int LED_PIN = 16;      // LED no GPIO16
const int LDR_PIN = 34;      // LDR no GPIO34 (ADC)

// Limiar para considerar ambiente escuro (ajuste conforme seu ambiente)
const int LIMIAR_LUZ = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);  // opcional para ADC

  Serial.println("Sistema iniciado.");
}

void loop() {
  int movimento = digitalRead(PIR_PIN);      // 0 ou 1
  int valorLuz = analogRead(LDR_PIN);        // 0 a 4095 no ESP32 ADC

  Serial.print("Movimento: ");
  Serial.print(movimento);
  Serial.print(" | Luz: ");
  Serial.println(valorLuz);

  // Se detectar movimento e estiver escuro, acende o LED
  if (movimento == HIGH && valorLuz < LIMIAR_LUZ) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(500);  // meio segundo entre leituras
}