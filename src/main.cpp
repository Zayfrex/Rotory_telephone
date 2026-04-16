#include <Arduino.h>
/*Rotary Phone
project developed by: Wellenzohn Alex (unibz), Rizzuto Matteo (unibz)

Project Description: Hacked a old rotary Telephone. The phone outputs sounds based on the dial numbers,
the original speaker is used by a simple NPN transitor circuit to run it low voltage. Every dial number
exerts a sound. The number 1 to 7 correspond to CDEFGAB Notes.
8: old nokia ringtone
9: starwars imperial walk sound
0 (10): some newstyle ringtone

-------Circuit description to rember where to plug in what-------------------------------------------
Rotary Counter every number on the dial corrisponds to the number of high pulses >> ex. 5 = 5 high pulses

Rotary Plate > contacts always closed open for every number so --> 5 = 5 times opening button.
button pull up so when number gets imputted we get high signals

Switch for ISR pulled down to gnd

repo git: https://github.com/Zayfrex/Rotory_telephone
*/

#define SPEAKER_PIN 9
//#define ROTORY_PIN A5
#define START_Button 2  
#define ROTORY_PIN 3 
#define TONE_DURATION 500
#define DIAL_TIMEOUT 300
#define LDR_PIN A0
const int BOUNCING_FILTER = 100;

enum NOTES{
  C=1,
  D,
  E,
  F,
  G,
  A,
  B,
  NOKIARINGTONE,
  STARWARS,
  RINGTONE,
};

bool start_flag = false;
volatile bool start_stop_flag_reading = false; // declare it as volatile, because i change it in the interrupt
volatile bool soundEnable = false;

bool number_flag = false;
unsigned long timestamp_number_button = 0;

uint8_t counter_number = 0;

double emaValue=0.0;
float alpha = 0.1;

uint16_t rotoryValue = 0;
uint8_t volts = 0;

void playMelody(int );
void start_reading();
void start_counting();
void playNote(uint8_t,int);
void nokiaringtone(int);
void starwars(int);
void classicalRingtone(int);

void setup() {
  Serial.begin(76800);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(ROTORY_PIN, INPUT);
  pinMode(START_Button, INPUT);
  pinMode(LDR_PIN,INPUT);
  
  attachInterrupt(digitalPinToInterrupt(START_Button), start_reading, CHANGE);
  playMelody(SPEAKER_PIN);
}

void loop() { 
  uint16_t ldrval = analogRead(LDR_PIN);
  emaValue = (alpha*ldrval)+(1-alpha)*emaValue; // filter ldr value
  //Serial.println(emaValue);
  if(emaValue <= 1023 && emaValue >= 850){ // this values could be false for lighter room (working in the dark)
    soundEnable = false;
  }
  else{
    soundEnable = true;
  }
  start_counting();

  // Only read counter after dial has stopped pulsing after the DIAL_TIMEOUT
  if(counter_number && (millis() - timestamp_number_button > DIAL_TIMEOUT)){
    playNote(counter_number,SPEAKER_PIN);
    delay(500); //avoid overlapping ---> hate to see delays but faster alternative (cheap)
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
  int notes[] = {
    440, 440, 440,      // A A A
    349, 523, 440,      // F C A
    349, 523, 440,      // F C A

    659, 659, 659,      // E E E
    698, 523, 415, 349, // F C G# F
    523, 440            // C A
  };

  int durations[] = {
    500, 500, 500,
    350, 150, 600,
    350, 150, 800,

    500, 500, 500,
    350, 150, 500, 500,
    350, 800
  };

  int length = sizeof(notes) / sizeof(notes[0]);

  for (int i = 0; i < length; i++) {
    tone(pin, notes[i], durations[i]);
    delay(durations[i] * 1.2);  // slight gap for clarity
    noTone(pin);
  }}

void classicalRingtone(int pin) {
  int notes[] = {
    523, 659, 784, 659,   // C E G E
    587, 698, 880, 698,   // D F A F
    659, 784, 988, 784,   // E G B G
    698, 880, 1047, 880,  // F A C(high) A

    784, 659, 523, 659,   // G E C E
    698, 587, 494, 587,   // F D B D
    523, 494, 440, 494,   // C B A B
    523                   // final note
  };

  int durations[] = {
    200, 200, 300, 200,
    200, 200, 300, 200,
    200, 200, 300, 200,
    200, 200, 400, 300,

    200, 200, 300, 200,
    200, 200, 300, 200,
    200, 200, 300, 200,
    600
  };

  int length = sizeof(notes) / sizeof(notes[0]);

  for (int i = 0; i < length; i++) {
    tone(pin, notes[i]);
    delay(durations[i]);
    noTone(pin);
    delay(40);
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

    case NOTES::NOKIARINGTONE:
      nokiaringtone(pin);
      break;

    case NOTES::STARWARS:
      starwars(pin);
      break;

    case NOTES::RINGTONE:
      classicalRingtone(pin);
      break;

    default:
      noTone(pin);
      break;
  }
}

void start_reading()
{
  static unsigned long timestamp_start_reading = 0; // static variable

  //Ignore if not allowed
  if(!soundEnable)
    return;

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