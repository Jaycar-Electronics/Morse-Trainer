#ifndef _MORSE_TABLE_H_
#define _MORSE_TABLE_H_

const int dit_unit = 1;
const int dah_unit = 3;
const int char_space = 1;
const int word_space = 7;
const long default_unit_timing = 100;

#include <Arduino.h>

#define S 0  //dit
#define L 1  //dah
#define X -1 //don't care

void sendStringToBuzzer(String &str, const int buzzer, int timing = default_unit_timing);
char getCharFromCode(const short *codePtr);
short *getCodeFromChar(char c);
short *getCharFromEncodedString(const char *str);
short *getCharFromEncodedString(String &str);

/*
#define QUEUE_SIZE 30

class MorseListener
{
  String *target;
  short oldSignal;
  long lastMark;
  int available;
  int queue[QUEUE_SIZE] = {0};

  void calculateAvailable();

public:
  MorseListener(String *t) : target(t);
  {
    startMark = 0;
  }

  void watch(int digitalPin);
  void addToQueue(int v);
  int removeFromQueue();
  short available();
};

*/

const short morse_table[36][5] = {
    {S, L, X, X, X}, // a
    {L, S, S, S, X}, // b
    {L, S, L, S, X}, // c
    {L, S, S, X, X}, // d
    {S, X, X, X, X}, // e
    {S, S, L, S, X}, // f
    {L, L, S, X, X}, // g
    {S, S, S, S, X}, // h
    {S, S, X, X, X}, // i
    {S, L, L, L, X}, // j
    {L, S, L, X, X}, // k
    {S, L, S, S, X}, // l
    {L, L, X, X, X}, // m
    {L, S, X, X, X}, // n
    {L, L, L, X, X}, // o
    {S, L, L, S, X}, // p
    {L, L, S, L, X}, // q
    {S, L, S, X, X}, // r
    {S, S, S, X, X}, // s
    {L, X, X, X, X}, // t
    {S, S, L, X, X}, // u
    {S, S, S, L, X}, // v
    {S, L, L, X, X}, // w
    {L, S, S, L, X}, // x
    {L, S, L, L, X}, // y
    {L, L, S, S, X}, // z
    {L, L, L, L, L}, // 0
    {S, L, L, L, L}, // 1
    {S, S, L, L, L}, // 2
    {S, S, S, L, L}, // 3
    {S, S, S, S, L}, // 4
    {S, S, S, S, S}, // 5
    {L, S, S, S, S}, // 6
    {L, L, S, S, S}, // 7
    {L, L, L, S, S}, // 8
    {L, L, L, L, S}, // 9
};

#endif