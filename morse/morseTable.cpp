#include "morseTable.h"
#include <Arduino.h>

short *getCodeFromChar(char c)
{

  if (c >= 'A' && c <= 'Z')
    c += ('a' - 'A'); //make lower case;

  if (c >= 'a' && c <= 'z')
  {
    c -= 'a';
    return morse_table[c];
  }

  if (c >= '0' && c <= '9')
  {
    c -= '0';
    c += 26; //skip past the alphabet
    return morse_table[c];
  }

  return 0;
}

char getCharFromCode(const short *code)
{
  //check each morse_table row
  for (short i = 0; i < 36; i++)
  {
    // check each letter of code;
    for (short j = 0; j < 5; j++)
    {
      if (code[j] != morse_table[i][j])
        break;

      if (j == 4)
      { //all characters match
        return (i < 26 ? i + 'a' : i - 26 + '0');
      }
    }
  }

  return 0;
}

void sendStringToBuzzer(String &s, const int buzzer, int timing)
{

  for (int i = 0; i < s.length(); i++)
  {
    if (s[i] == ' ')
    {
      delay(timing * word_space);
      continue;
    }

    short *p = getCodeFromChar(s[i]);

    if (p == 0)
      continue;

    //output the code for this letter
    for (int j = 0; j < 5; j++)
    {
      if (p[j] == -1)
        break;

      digitalWrite(buzzer, HIGH);
      delay(timing * (p[j] ? dah_unit : dit_unit));
      digitalWrite(buzzer, LOW);
      delay(timing >> 2);
    }
    delay(timing * char_space);
  }
}
short *getCharFromEncodedString(String &str)
{
  return getCharFromEncodedString(str.c_str());
}

short *getCharFromEncodedString(const char *str)
{
  short buf[5] = {0};
  short bufIndex = 0;
  for (short i = 0; str[i] != '\0'; i++)
  {
    if (str[i] == '.')
    {
      buf[bufIndex++] = S;
    }
    else if (str[i] == '-')
    {
      buf[bufIndex++] = L;
    }
    else
    {
      buf[bufIndex++] = X;
    }
  }

  return getCharFromCode(buf);
}
/*
void MorseListener::watch(int digitalPin)
{
  short reading = digitalRead(digitalPin);

  if (reading == oldSignal)
    return;

  int duration;

  if (reading == LOW) //went from high to low
  {
    duration = millis() - lastMark;
  }
  else
  { //low to high
    duration = -(millis() - lastMark);
  }
  addToQueue(duration);
  lastMark = millis();
  oldSignal = reading;

}

void MorseListener::addToQueue(int val)
{
  queue[_insert++] = val;
  _insert %= QUEUE_SIZE;
  calculateAvailable();
}

int MorseListener::removeFromQueue()
{

  if (_read == _insert)
  {
    return -1;
  }

  int v = queue[_read++];
  _read %= QUEUE_SIZE;

  calculateAvailable();
  return v;
}
void MorseListener::calculateAvailable()
{
  queueAvailable = QUEUE_SIZE + (_insert - _read);
  queueAvailable %= QUEUE_SIZE;
}
short MorseListener::available()
{
  return available;
}
*/