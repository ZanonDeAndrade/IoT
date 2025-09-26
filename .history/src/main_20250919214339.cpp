#define PIR_PIN GPIO_NUM_4
#define LED_PIN GPIO_NUM_16

configuração vazia() {
  pinMode(PIR_PIN, ENTRADA);  
  pinMode(LED_PIN, SAÍDA);
}

loop vazio() {
  esp_sleep_enable_ext0_wakeup(PIR_PIN, 1);   
  digitalWrite(LED_PIN1, ALTO);
  atraso(5000);
  esp_deep_sleep_start();}/