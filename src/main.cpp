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
#define TONE_DURATION 500
#define DIAL_TIMEOUT 300

enum NOTES{
  C=1,
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

void playMelody(int );
void start_reading();
void start_counting();
void playNote(uint8_t,int);
void nokiaringtone(int );
void starwars(int );

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

  // Only read counter after dial has stopped pulsing after the DIAL_TIMEOUT
  if(counter_number && (millis() - timestamp_number_button > DIAL_TIMEOUT)){
    playNote(counter_number,SPEAKER_PIN);
    delay(500);
    counter_number = 0;
  }
}

//Turn On Melody to test the phone
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


void nokiaringtone(int pin) {
  int notes[]     = {659, 587, 370, 415, 554, 494, 294, 330, 494, 440, 277, 330, 440};
  int durations[] = {125, 125, 250, 250, 125, 125, 250, 250, 125, 125, 250, 250, 500};

  for (int i = 0; i < 13; i++) {
    tone(pin, notes[i]);
    delay(durations[i]);
    noTone(pin);
    delay(30);
  }
}

void starwars(int pin) {
  int notes[]     = {440, 440, 440, 349, 523, 440, 349, 523, 440, 659, 659, 659, 698, 523, 415, 349, 523, 440};
  int durations[] = {300, 300, 300, 200, 100, 300, 200, 100, 500, 300, 300, 300, 200, 100, 300, 200, 100, 500};

  for (int i = 0; i < 18; i++) {
    tone(pin, notes[i]);
    delay(durations[i]);
    noTone(pin);
    delay(30);
  }
}


void playNote(uint8_t note,int pin){ //IF needed --------!!!!! need to add the counter reset 
  switch (note)
  {
    case NOTES::C:
      tone(pin, 262,TONE_DURATION);
      break;

    case NOTES::D:
      tone(pin, 294,TONE_DURATION);
      break;

    case NOTES::E:
      tone(pin, 330,TONE_DURATION);
      break;

    case NOTES::F:
      tone(pin, 349,TONE_DURATION);
      break;

    case NOTES::G:
      tone(pin, 392,TONE_DURATION);
      break;

    case NOTES::A:
      tone(pin, 440,TONE_DURATION);
      break;

    case NOTES::B:
      tone(pin, 494,TONE_DURATION);
      break;

    case 8:
      nokiaringtone(pin);
      break;

    case 9:
      starwars(pin);
      break;

    default:
      noTone(pin);
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
  // Only count pulses while start button is active
  if (!start_stop_flag_reading) 
    return;

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