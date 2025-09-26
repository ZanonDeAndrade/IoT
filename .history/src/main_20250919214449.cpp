#include <Arduino.h>

#define PIR_PIN GPIO_NUM_4    
#define LED_PIN GPIO_NUM_16   // Pino do LED

void setup(){
  pinMode(PIR_PIN, INPUT);   // Configura PIR como entrada
  pinMode(LED_PIN, OUTPUT);  // Configura LED como saída
}

void loop() {
  // Habilita despertar quando PIR detectar movimento (nível alto = 1)
  esp_sleep_enable_ext0_wakeup(PIR_PIN, 1);   

  // Acende LED por 5 segundos
  digitalWrite(LED_PIN, HIGH);
  delay(5000);

  // Entra em modo deep sleep
  esp_deep_sleep_start();  
}
