#include <Encoder.h>
#include <avr/io.h>//for fast PWM

//encoder library setting
//#define ENCODER_OPTIMIZE_INTERRUPTS //contermeasure of encoder noise
#define ENCODER_OPTIMIZE_INTERRUPTS

//OLED display setting
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#define OLED_ADDRESS 0X3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

typedef struct {
  char title[12];
  char subtitle[12];
  char param1[12];
  char param2[12];
  char param3[12];
} Effect;

// presets on FV-1 chip
const Effect preset[] PROGMEM = {
  {"Chorus", "Reverb", "rev mix", "cho rate", "cho mix"},
  {"Flange", "Reverb", "rev mix", "flng rate", "flng mix"},
  {"Tremolo", "Reverb", "rev mix", "trem rate", "trem mix"},
  {"Pitch", "Shift", "pitch", "-", "-"},
  {"Pitch", "Echo", "pitch", "echo delay", "echo mix"},
  {"No Effect", " ", "-", "-", "-"},
  {"Reverb", "1", "rev time", "hf filter", "lf filter"},
  {"Reverb", "2", "rev time", "hf filter", "lf filter"},
};

// add your own effect names from rom1
const Effect rom1[] PROGMEM = {
  {"rom1_00_0", "rom1_00_1", "param1", "param1", "param3"},
  {"rom1_01_0", "rom1_01_1", "param1", "param1", "param3"},
};

// add your own effect names from rom2
const Effect rom2[] PROGMEM = {
  {"rom2_00_0", "rom2_00_1", "param1", "param1", "param3"},
  {"rom2_01_0", "rom2_01_1", "param1", "param1", "param3"},
};

Encoder myEnc(2, 4);
int oldPosition = -999;
int newPosition = -999;
int i = 1;

int POT0 = 150;
int POT1 = 150;
int POT2 = 150;

bool old_sw = 0;
bool sw = 0;
byte bank = 0;
Effect currentFx = preset[0];

bool disp_refresh = 0;
bool disp_ref_count = 0;

void setup() {
  // OLED library setting
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // pin mode setting
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  // fast pwm setting
  TCCR2B &= B11111000;
  TCCR2B |= B00000001;
  TCCR1B &= B11111000;
  TCCR1B |= B00000001;

  disp_ref_count = millis();
}

void loop() {
  // change bank(ROM) by push button
  old_sw = sw;
  sw = digitalRead(12);

  if (sw == 1 && old_sw == 0) {
    bank++;
    disp_refresh = 1;
    if (bank >= 3) {
      bank = 0;
    }

    if (bank == 2) {
      delay(1000);
      digitalWrite(9, LOW);
      delay(5);
      digitalWrite(8, HIGH);
      delay(5);
      digitalWrite(13, HIGH);
      delay(5);
      digitalWrite(13, LOW);
      delay(5);
      digitalWrite(13, HIGH);
    }
    else if (bank == 1) {
      delay(1000);
      digitalWrite(8, LOW);
      delay(5);
      digitalWrite(9, HIGH);
      delay(5);
      digitalWrite(13, HIGH);
      delay(5);
      digitalWrite(13, LOW);
      delay(5);
      digitalWrite(13, HIGH);
      delay(5);
    }
    else if (bank == 0) {
      delay(1000);
      digitalWrite(13, LOW);
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
    }
  }

  newPosition = myEnc.read();
  if ((newPosition - 3) / 4 > oldPosition / 4) {
    oldPosition = newPosition;
    i = i - 1;
    disp_refresh = 1;
    if (i <= -1) {
      i = 7;
    }
  }

  else if ((newPosition + 3) / 4 < oldPosition / 4) {
    oldPosition = newPosition;
    i = i + 1;
    disp_refresh = 1;
    if (i >= 8) {
      i = 0;
    }
  }
  

  i = constrain(i, 0, 7);

  // PWM value calc
  POT0 = (analogRead(0) + analogRead(3));
  POT1 = (analogRead(1) + analogRead(6));
  POT2 = (analogRead(2) + analogRead(7));
  POT0 = map(POT0, 0, 1023, 0, 150);
  POT1 = map(POT1, 0, 1023, 0, 150);
  POT2 = map(POT2, 0, 1023, 0, 150);

  // select fv1 effect number
  digitalWrite(7, bitRead(i, 0));
  digitalWrite(6, bitRead(i, 1));
  digitalWrite(5, bitRead(i, 2));

  // PWM output
  analogWrite(3, POT2);
  analogWrite(10, POT1);
  analogWrite(11, POT0);

  // display refresh freq
  if ((disp_refresh == 1) || (millis() >= disp_ref_count + 100)) {
    disp_refresh = 0;
    disp_ref_count = millis();
    displayOut();
  }
}

void displayOut() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  switch(bank) {
    case 0:
      memcpy_P(&currentFx, &preset[i], sizeof(Effect));
      break;
    case 1:
      memcpy_P(&currentFx, &rom1[i], sizeof(Effect));
      break;
    case 2:
      memcpy_P(&currentFx, &rom2[i], sizeof(Effect));
      break;      
  }
  

  display.setCursor(0, 0);
  display.print(currentFx.title);

  display.setCursor(0, 16);
  display.print(currentFx.subtitle);

  display.setTextSize(0);
  display.setCursor(120, 0);
  display.print(i);

  display.setCursor(0, 34);
  display.print(currentFx.param1);

  display.setCursor(0, 44);
  display.print(currentFx.param2);

  display.setCursor(0, 54);
  display.print(currentFx.param3);

  display.fillRoundRect(74, 37, POT0 / 3, 5, 2, WHITE);
  display.fillRoundRect(74, 47, POT1 / 3, 5, 2, WHITE);
  display.fillRoundRect(74, 57, POT2 / 3, 5, 2, WHITE);

  display.display();

}
