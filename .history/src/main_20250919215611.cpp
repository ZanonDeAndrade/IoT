#include <Arduino.h>

// Definições de pinos
#define PIR_PIN 4       // Saída do PIR (AM312)
#define LDR_PIN 34      // Entrada analógica para o divisor de tensão com LDR
#define LED_PIN 18      // LED

// Limiar de luz (ajuste após medir os valores no Serial Monitor)
int LIMIAR_LUZ = 2500; // Quanto maior, mais "escuro" precisa estar

void setup() {
    pinMode(PIR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    Serial.begin(115200);
    Serial.println("Sistema de Luz Noturna iniciado...");
}

void loop() {
    // Leitura do PIR
    int movimento = digitalRead(PIR_PIN);

    // Leitura do LDR (0 a 4095 no ESP32)
    int luz = analogRead(LDR_PIN);

    // Debug no Serial
    Serial.print("Movimento: ");
    Serial.print(movimento);
    Serial.print(" | Luz: ");
    Serial.println(luz);

    // Acende LED somente se houver movimento e ambiente estiver escuro
    if (movimento == HIGH && luz > LIMIAR_LUZ) {
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }

    delay(200); // atraso pequeno para estabilidade
}
