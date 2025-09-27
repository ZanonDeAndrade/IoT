const int pirPin = 2; // GPIO pin connected to the OUT pin of the PIR sensor

void setup() {
  Serial.begin(115200);
  pinMode(pirPin, INPUT);
}

void loop() {
  int pirState = digitalRead(pirPin);

  if (pirState == HIGH) {
    Serial.println("Motion detected!");
    // Add your desired actions when motion is detected
  } else {
    Serial.println("No motion");
    // Add your desired actions when no motion is detected
  }

  delay(1000); // Adjust the delay based on your project requirements
}