#include "decoder.hpp"

bool MorseCodeBuffer::Enqueue(MorseCodeElement element)
{
  int new_tail = (m_tail + 1) % m_size;

  // Is full?
  if (new_tail == m_head)
  {
    return false;
  }

  m_tail = new_tail;
  m_buffer[m_tail] = element;

  return true;
}
bool MorseCodeBuffer::TryDequeue(MorseCodeElement *element)
{
  // Is empty?
  if (m_head == m_tail)
  {
    return false;
  }

  *element = m_buffer[m_head];
  m_head = (m_head + 1) % m_size;
  return true;
}

int MorseCodeBuffer::GetCount()
{
  if (m_head == m_tail)
  {
    return 0;
  }

  return (m_tail - m_head + m_size) % m_size;
}

bool AdaptiveLogicLevelProcessor::process(int sensorValue, int *digitalInputValue)
{
  unsigned long currentTime = millis();
  /*
  // Re-calibrate sensor value range
  if (currentTime - m_sensorCalibrationTime > 5000)
  {
    if (m_sensorMinValue < m_sensorMaxValue)
    {

      if (m_sensorMaxValue - m_sensorMinValue > 20)
      {
        m_sensorMedianValue = m_sensorMinValue + (m_sensorMaxValue - m_sensorMinValue) / 2;
        m_calibrated = true;
      }
      else
      {
        Serial.println();
        Serial.print("Unreliable LOW/HIGH: ");
        Serial.print(m_sensorMinValue);
        Serial.print(' ');
        Serial.print(m_sensorMaxValue);
        Serial.println();
        m_calibrated = false;
      }
    }

    m_sensorMaxValue = 0;
    m_sensorMinValue = 1023;
    m_sensorCalibrationTime = currentTime;
  }

  if (m_sensorMinValue > sensorValue)
  {
    m_sensorMinValue = sensorValue;
  }

  if (m_sensorMaxValue < sensorValue)
  {
    m_sensorMaxValue = sensorValue;
  }

  if (!m_calibrated)
  {
    return false;
  }
  */

  // *digitalInputValue = sensorValue > m_sensorMedianValue ? HIGH : LOW;
  *digitalInputValue = sensorValue;
  return true;
}

bool MorseCodeElementProcessor::process(int newSignal, MorseCodeElement *element)
{
  unsigned long currentTime = millis();
  unsigned long elapsed;
  bool shouldBuffer = false;

  element->m_signal = Signal::NOISE;

  // If previous status was OFF and now it is ON
  if (m_previousSignal == LOW && newSignal == HIGH)
  {
    elapsed = currentTime - m_previousTime;
    element->m_duration = elapsed;

    if (m_sevenUnitMinValue <= elapsed)
    {
      element->m_signal = Signal::LONGGAP;
      shouldBuffer = true;
    }
    else if (m_threeUnitMinValue <= elapsed && elapsed <= m_threeUnitMaxValue)
    {
      element->m_signal = Signal::GAP;
      shouldBuffer = true;
    }
    else if (m_oneUnitMinValue <= elapsed && elapsed <= m_oneUnitMaxValue)
    {
      element->m_signal = Signal::ELEMENTGAP;
      shouldBuffer = true;
    }
    else
    {
      element->m_signal = Signal::NOISE;
      shouldBuffer = true;
    }

    m_previousSignal = HIGH;
    m_previousTime = currentTime;
  }
  else if (m_previousSignal == HIGH && newSignal == LOW)
  {
    elapsed = currentTime - m_previousTime;
    element->m_duration = elapsed;

    if (m_threeUnitMinValue <= elapsed && elapsed <= m_threeUnitMaxValue)
    {
      element->m_signal = Signal::DAH;
      shouldBuffer = true;
    }
    else if (m_oneUnitMinValue <= elapsed && elapsed <= m_oneUnitMaxValue)
    {
      element->m_signal = Signal::DIT;
      shouldBuffer = true;
    }
    else
    {
      element->m_signal = Signal::NOISE;
      shouldBuffer = true;
    }

    m_previousSignal = LOW;
    m_previousTime = currentTime;
  }

  return shouldBuffer;
}

bool MorseCodeProcessor::process(Signal input, char *output)
{
  bool completed = false;

  if (!m_error && input == Signal::DIT)
  {
    if (m_start == m_index)
    {
      m_error = true;
    }
    else
    {
      m_end = m_index;
      m_index = m_start + (m_end - m_start) / 2;
    }
  }
  else if (!m_error && input == Signal::DAH)
  {
    if (m_end == m_index)
    {
      m_error = true;
    }
    else
    {
      m_start = m_index + 1;
      m_index = m_start + (m_end - m_start) / 2;
    }
  }
  else if (input == Signal::GAP || input == Signal::LONGGAP)
  {
    completed = !m_error && tree[m_index] != 0;

    if (completed)
    {
      output[0] = tree[m_index];
      output[1] = '\0';

      if (input == Signal::LONGGAP)
      {
        output[1] = ' ';
        output[2] = '\0';
      }
    }

    reset();
  }

  m_previousInput = input;

  return completed;
}
