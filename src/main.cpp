#include <Arduino.h>

#define SPEAKER_PIN 9


void setup() {
  pinMode(SPEAKER_PIN, OUTPUT);
}

void loop() {
  analogWrite(SPEAKER_PIN, 128); // 50% duty cycle
}
