/*
Copyright (c) 2018 Paul Austin.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include <MicroBit.h>
#include "MicroBitUARTServiceFixed.h"
#include "TeakTask.h"
#include "TrashbotsController.h"

extern MicroBit uBit;

//------------------------------------------------------------------------------
// SensorTask - A task for directly using the sensors
class SensorTask : public TeakTask {
public:
    SensorTask();
    void Event(MicroBitEvent event);
    void SelectSensor(int index);
    int m_sensor;
    enum {
      kSensorFirst,
      kSensorLevel = kSensorFirst,
      kSensorAccel,
      kSensorGyro,
      kSensorEncoder,
      kSensorCompass,
      kSensorLast = kSensorCompass,
    };
};

SensorTask gSensorTask;
TeakTask* gpSensorTask = &gSensorTask;

const int kLevelSensorImg = PBMAP(
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(1, 0, 1, 0, 1),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_FRAME_COUNT(1));

const int kAccelSensorImg = PBMAP(
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_ROW(1, 1, 0, 1, 1),
    PBMAP_ROW(1, 0, 1, 0, 1),
    PBMAP_ROW(1, 1, 0, 1, 1),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_FRAME_COUNT(1));

const int kGyroSensorImg = PBMAP(
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_ROW(1, 0, 1, 0, 1),
    PBMAP_ROW(1, 1, 1, 1, 1),
    PBMAP_ROW(1, 0, 1, 0, 1),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_FRAME_COUNT(1));

const int kEncoderSensorImg = PBMAP(
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_FRAME_COUNT(1));

const int kCompassSensorImg = PBMAP(
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(1, 1, 1, 1, 1),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_FRAME_COUNT(1));

int sensorImages[] = {
  kLevelSensorImg,
  kAccelSensorImg,
  kGyroSensorImg,
  kEncoderSensorImg,
  kCompassSensorImg,
};

SensorTask::SensorTask()
{
    m_sensor = kSensorLevel;
    SelectSensor(m_sensor);
}

void SensorTask::SelectSensor(int index)
{
    m_image = sensorImages[index];
}

void SensorTask::Event(MicroBitEvent event)
{
    if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
        if (event.source == MICROBIT_ID_BUTTON_A) {
          if (m_sensor > kSensorFirst) {
            m_sensor -= 1;
          } else {
            m_sensor = kSensorLast;
          }
        } else if (event.source == MICROBIT_ID_BUTTON_B) {
          if (m_sensor < kSensorLast) {
            m_sensor += 1;
          } else {
            m_sensor = kSensorFirst;
          }
        }
        SelectSensor(m_sensor);
    } else if (event.source == MICROBIT_ID_TASK_SWAP) {
        gTaskManager.SwitchTo(gpTopMenuTask);
    } else if (event.source == MICROBIT_ID_TIMER) {
    }
}
