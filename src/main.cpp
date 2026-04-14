#include <Arduino.h>

#define SPEAKER_PIN 9

void playMelody(int pin);

void setup() {
  pinMode(SPEAKER_PIN, OUTPUT);
}

void loop() {
  playMelody(SPEAKER_PIN);
  delay(2000);
}

void playMelody(int pin) {
  int notes[] = {440, 494, 523, 587};
  int durations[] = {200, 200, 200, 400};

  for (int i = 0; i < 4; i++) {
  tone(pin, notes[i]);
  delay(durations[i]);
  noTone(pin);
  delay(50);
  }
}