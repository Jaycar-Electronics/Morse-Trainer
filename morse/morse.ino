
#include "morseTable.h"
#include "decoder.hpp"
//#include "Adafruit_SH1106.h"
#include <U8g2lib.h>
#include "EEPROM.h"

const int morseTiming = 100;

const short button_pin = 8;
const short buzzer_pin = A4;
const short signal_pin = 2;

//U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(
//    U8G2_R0, A1, A0, -1);

U8X8_SH1106_128X64_NONAME_4W_HW_SPI oled(A1, A0);

short mode = 0;

//we have 4 modes:

// buzzer/mic serial monitor - pass through
// button/oled - practise mode
// listen/buzzer/button - talkback

//back and forth through the serial monitor
#define MODE_SERIAL 1
#define MODE_TALK 2
#define MODE_PRACTISE 3

const short mode_address = 0xAA;

String listenBuffer = "";
String outputBuffer = "";

constexpr char MorseCodeProcessor::tree[];

MorseCodeBuffer buffer(BUFFER_SIZE);
MorseCodeProcessor morseCodeProcessor;
MorseCodeElementProcessor morseCodeElementProcessor(UNIT_LENGTH);

char *output = new char[3];

SIGNAL(TIMER0_COMPA_vect)
{
  cli();
  MorseCodeElement e;
  //invert the signal
  if (morseCodeElementProcessor.process(digitalRead(signal_pin)? LOW:HIGH, &e))
  {
    buffer.Enqueue(e);
  }
  sei();
}

void setup()
{
  Serial.begin(9600);
  outputBuffer.reserve(15);
  listenBuffer.reserve(15);
  oled.begin();
  oled.setFlipMode(1);
  oled.setFont(u8x8_font_artossans8_r);
  oled.drawString(0, 0, "Hello World");

  pinMode(button_pin, INPUT_PULLUP);
  pinMode(signal_pin, INPUT);
  pinMode(buzzer_pin, OUTPUT);

  delay(500); //wait half a second

  if (digitalRead(button_pin) == LOW)
  { //if button is pressed
    oled.clear();
    mode = showMenuFunction();
  }
  else
  {
    mode = EEPROM.read(mode_address);
  }

  switch (mode)
  {
  case MODE_SERIAL:
    Serial.println(F("Serial mode enabled"));
  }

  //enable watch vector function
  cli();
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
  sei();
}

bool TryDequeueSafe(MorseCodeElement *element)
{
  // Dequeue item from the buffer while disabling interrupt
  // so that it doesn't corrupt buffer status
  cli();
  bool result = buffer.TryDequeue(element);
  sei();

  return result;
}

void loop()
{
  switch (mode)
  {
  case MODE_SERIAL:
    serialModeLoop();
  case MODE_TALK:
    talkModeLoop();
  case MODE_PRACTISE:
    practiseModeLoop();
  default:
    serialModeLoop();
  }
}

void serialModeLoop()
{
  MorseCodeElement e;
  while (TryDequeueSafe(&e))
  {
    if (morseCodeProcessor.process(e.m_signal, output))
    {
      Serial.print(output);
    }
    if (e.m_signal == Signal::LONGGAP)
    {
      Serial.print(" ");
    }
  }

  if (Serial.available())
  {
    outputBuffer = Serial.readString();
    if (outputBuffer.length() > 0)
    {
      Serial.print(">");
      Serial.println(outputBuffer);
      sendStringToBuzzer(outputBuffer, buzzer_pin);
    }
  }
}
void talkModeLoop()
{
}

void practiseModeLoop()
{
}

short showMenuFunction()
{

  oled.drawString(0, 0, "Menu Mode");
  oled.drawString(0, 1, "Open Serial");
  oled.drawString(0, 2, "[ 9600 ] Baud");

  while (1)
  {
    Serial.println(F("Menu"));
    Serial.println(F("-------"));
    Serial.println(F("1. Serial mode"));
    Serial.println(F("2. Talk/Reply mode"));
    Serial.println(F("3. Practise mode"));
    Serial.println(F("-------"));
    Serial.println();

    while (!Serial.available())
      ;

    String input = Serial.readString();
    short val = input.toInt();
    if (val >= MODE_SERIAL && val <= MODE_PRACTISE)
    {
      Serial.print(F("Setting mode: "));
      Serial.println(val, DEC);
      Serial.println();
      EEPROM.write(mode_address, val);
      return val;
    }
    Serial.println(F("Not a mode"));
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
  }
}
/*
void captureChange()
{
  if (digitalRead(3) == HIGH)
  {
    startMark = millis();
    return;
  }
  else if (startMark != 0)
    endMark = millis();

  pushMark(startMark, endMark);
}

void pushMark(long start, long end)
{
}
*/
/* -----------------------------

  while (Serial.available())
  {
    char c = Serial.read();
    short *p = getCodeFromChar(c);

    if (p == 0)
      continue;

    for (int i = 0; i < 5; i++)
    {
      if (p[i] == -1)
        break;
      Serial.print(p[i] ? '-' : '.');
    }
    Serial.print('/');
  }
  ------------------------ */