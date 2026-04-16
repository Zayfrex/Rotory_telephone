#include <Arduino.h>
/*Rotory Phone
Rotory Counter every number on the dial corrisponds to the number of high pulses >> ex. 5 = 5 high pulses

Rotory Plate > contacts always closed open for every number so --> 5 = 5 times opening button.
button pull up so when number gets imputted we get high signals

Switch for ISR pulled down to gnd
*/

#define SPEAKER_PIN 9
//#define ROTORY_PIN A5
#define START_Button 2  
#define ROTORY_PIN 3 

enum NOTES{
  C,
  D,
  E,
  F,
  G,
  A,
  B,
};

bool start_flag = false;
volatile bool start_stop_flag_reading = false; // deklare it as volatile, because i change it in the interrupt

bool number_flag = false;
unsigned long timestamp_number_button = 0;

uint8_t counter_number = 0;

const int BOUNCING_FILTER = 100;


uint16_t rotoryValue = 0;
uint8_t volts = 0;

void playMelody(int pin);
void start_reading();
void start_counting();

void setup() {
  Serial.begin(76800);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(ROTORY_PIN, INPUT);
  pinMode(START_Button, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(START_Button), start_reading, CHANGE);
  playMelody(SPEAKER_PIN);
}

void loop() {
  start_counting();
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

void playNote(uint8_t note,int pin){ //IF needed 
  switch (note)
  {
    case NOTES::C:
      tone(pin, 262);
      break;

    case NOTES::D:
      tone(pin, 294);
      break;

    case NOTES::E:
      tone(pin, 330);
      break;

    case NOTES::F:
      tone(pin, 349);
      break;

    case NOTES::G:
      tone(pin, 392);
      break;

    case NOTES::A:
      tone(pin, 440);
      break;

    case NOTES::B:
      tone(pin, 494);
      break;

    default:
      noTone(8);
      break;
  }
}

void start_reading()
{
  static unsigned long timestamp_start_reading = 0; // static variable

  if (millis() - timestamp_start_reading > BOUNCING_FILTER)
  {
    timestamp_start_reading = millis();
    start_stop_flag_reading = !start_stop_flag_reading;

    Serial.println("Change State");
  }
}

void start_counting()
{
  uint8_t pin_state = digitalRead(ROTORY_PIN);
  if (pin_state == HIGH && number_flag == false)
  {
    if (millis() - timestamp_number_button > BOUNCING_FILTER)
    {
      timestamp_number_button = millis();
      counter_number++;
      Serial.println(counter_number);
      number_flag = true;
    }
  }
  else if (pin_state == LOW)
  {
    number_flag = false;
  }
}