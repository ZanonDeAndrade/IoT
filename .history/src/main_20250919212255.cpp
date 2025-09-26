#define PIR_PIN GPIO_NUM_4
#define LED_PIN GPIO_NUM_16

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Configura o ESP32 para acordar com sinal HIGH no pino do PIR
  esp_sleep_enable_ext0_wakeup(PIR_PIN, 1);  

  // Acende o LED por 5 segundos
  digitalWrite(LED_PIN, HIGH);
  delay(5000);

  // Coloca o ESP32 em sono profundo
  esp_deep_sleep_start();  
}

void loop() {
  // Nunca será chamado após deep sleep
}