#define LDR_PIN 34  // Um pino ADC válido no ESP32

void setup() {
  Serial.begin(115200);
  pinMode(LDR_PIN, INPUT);  // opcional para ADC
}

void loop() {
  int valorLDR = analogRead(LDR_PIN);
  Serial.println(valorLDR);
  delay(500);
}
