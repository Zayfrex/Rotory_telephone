#include <Arduino.h>

#define SPEAKER_PIN 9
#define ROTORY_PIN A5

void playMelody(int pin);

void setup() {
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(ROTORY_PIN, INPUT);
  Serial.begin(9600);
}
uint16_t rotoryValue = 0;
uint8_t volts = 0;

void loop() {
  rotoryValue = analogRead(ROTORY_PIN);
  volts = rotoryValue * 5/1024;
  Serial.println(volts);
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