#ifndef _DECODER_H__
#define _DECODER_H__

#include "Arduino.h"

const int UNIT_LENGTH = 100;
const int BUFFER_SIZE = 5;

// -------------------------------------------
enum Signal : byte
{
  NOISE = 0,
  DIT = 1,
  DAH = 2,
  ELEMENTGAP = 3,
  GAP = 4,
  LONGGAP = 5
};
// -------------------------------------------
struct MorseCodeElement
{
  Signal m_signal;
  unsigned long m_duration;
};
// -------------------------------------------

class MorseCodeBuffer
{
  int m_size;
  int m_head;
  int m_tail;
  MorseCodeElement *m_buffer;

public:
  MorseCodeBuffer(int size)
  {
    // Use extra element to distinguish empty vs full.
    size++;

    m_size = size;
    m_head = 0;
    m_tail = 0;
    m_buffer = new MorseCodeElement[size];
  }

  bool Enqueue(MorseCodeElement element);

  bool TryDequeue(MorseCodeElement *element);

  int GetCount();
};
// ----------------------------------------------------------
class AdaptiveLogicLevelProcessor
{
  int m_sensorMinValue = 1023;
  int m_sensorMaxValue = 0;
  int m_sensorMedianValue = 511;
  unsigned long m_sensorCalibrationTime = 0;
  bool m_calibrated;

public:
  AdaptiveLogicLevelProcessor()
  {
    m_sensorMinValue = 1023;
    m_sensorMaxValue = 0;
    m_sensorMedianValue = 511;
    m_sensorCalibrationTime = 0;
  }

  bool process(int sensorValue, int *digitalInputValue);
};
// -------------------------------------------------------------------------
class MorseCodeElementProcessor
{
  unsigned long m_previousTime = 0;
  int m_previousSignal = LOW;

  int m_oneUnitMinValue;
  int m_oneUnitMaxValue;
  int m_threeUnitMinValue;
  int m_threeUnitMaxValue;
  int m_sevenUnitMinValue;
  int m_sevenUnitMaxValue;

public:
  MorseCodeElementProcessor(int unitLengthInMilliseconds)
  {
    m_oneUnitMinValue = (int)(unitLengthInMilliseconds * 0.5);
    m_oneUnitMaxValue = (int)(unitLengthInMilliseconds * 1.5);

    m_threeUnitMinValue = (int)(unitLengthInMilliseconds * 2.0);
    m_threeUnitMaxValue = (int)(unitLengthInMilliseconds * 4.0);

    m_sevenUnitMinValue = (int)(unitLengthInMilliseconds * 5.0);
    m_sevenUnitMaxValue = (int)(unitLengthInMilliseconds * 8.0);
  }

  bool process(int newSignal, MorseCodeElement *element);
};
// -----------------------------------------------------------------------------------
class MorseCodeProcessor
{
private:
  static const int TREE_SIZE = 255;
  static constexpr char tree[TREE_SIZE] = {
      '\0', '\0', '\0', '5', '\0', '\0', '\0', 'H', '\0', '\0', '\0', '4', '\0', '\0', '\0', 'S',
      '\0', '\0', '$', '\0', '\0', '\0', '\0', 'V', '\0', '\0', '\0', '3', '\0', '\0', '\0', 'I',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'F', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'U',
      '\0', '?', '\0', '\0', '\0', '_', '\0', '\0', '\0', '\0', '\0', '2', '\0', '\0', '\0', 'E',
      '\0', '\0', '\0', '&', '\0', '\0', '\0', 'L', '\0', '"', '\0', '\0', '\0', '\0', '\0', 'R',
      '\0', '\0', '\0', '+', '\0', '.', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'A',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'P', '\0', '@', '\0', '\0', '\0', '\0', '\0', 'W',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'J', '\0', '\'', '\0', '1', '\0', '\0', '\0', '\0',
      '\0', '\0', '\0', '6', '\0', '-', '\0', 'B', '\0', '\0', '\0', '=', '\0', '\0', '\0', 'D',
      '\0', '\0', '\0', '/', '\0', '\0', '\0', 'X', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'N',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'C', '\0', ';', '\0', '\0', '\0', '!', '\0', 'K',
      '\0', '\0', '\0', '(', '\0', ')', '\0', 'Y', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'T',
      '\0', '\0', '\0', '7', '\0', '\0', '\0', 'Z', '\0', '\0', '\0', '\0', '\0', ',', '\0', 'G',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'Q', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'M',
      '\0', ':', '\0', '8', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'O',
      '\0', '\0', '\0', '9', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '0', '\0', '\0', '\0'};

  bool m_error;
  int m_start;
  int m_end;
  int m_index;
  Signal m_previousInput;

  void reset()
  {
    m_error = false;
    m_start = 0;
    m_end = TREE_SIZE;
    m_index = (m_end - m_start) / 2;
  }

public:
  MorseCodeProcessor()
  {
    reset();
    m_previousInput = Signal::NOISE;
  }

  bool process(Signal input, char *output);
};

#endif