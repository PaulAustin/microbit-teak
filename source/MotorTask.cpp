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
#include "TBCDriver.h"

extern MicroBit uBit;

//------------------------------------------------------------------------------
// MotorTask - A task for direct control of the motors
class MotorTask : public TeakTask {
public:
    MotorTask();
    void Event(MicroBitEvent event);
};
MotorTask gMotorTask;
TeakTask* gpMotorTask = &gMotorTask;

const int kMotoBase = PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(1, 0, 1, 0, 1),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(1));

const int kMotorLeftForward  =  PBMAP(PBMAP_ROW(0, 1, 0, 0, 0), 0, 0, 0, 0, 0);
const int kMotorRightForward =  PBMAP(PBMAP_ROW(0, 0, 0, 1, 0), 0, 0, 0, 0, 0);
const int kMotorLeftBack     =  PBMAP(0, 0, 0, 0, PBMAP_ROW(0, 1, 0, 0, 0), 0);
const int kMotorRightBack    =  PBMAP(0, 0, 0, 0, PBMAP_ROW(0, 0, 0, 1, 0), 0);

MotorTask::MotorTask()
{
    m_note = ksNoteC4;
    m_image = kMotoBase;
}

bool m1State = false;
bool m2State = false;

void MotorTask::Event(MicroBitEvent event)
{
    if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
        m_image = kMotoBase;
        if (event.source == MICROBIT_ID_BUTTON_A) {
            m_image |= kMotorLeftForward;
            SetMotorPower(1, m1State ? 0 : -100);
            m1State = !m1State;
        } else if (event.source == MICROBIT_ID_BUTTON_B) {
            m_image |= kMotorRightForward;
            SetMotorPower(2, m2State ? 0 : 100);
            m2State = !m2State;
        } else if (event.source == MICROBIT_ID_BUTTON_AB) {
            m_image |= kMotorLeftForward;
            m_image |= kMotorRightForward;
        }
    } else if (event.source == MICROBIT_ID_TASK_SWAP) {
        // Shut down an pop to top
        SetMotorPower(1, 0);
        SetMotorPower(2, 0);
        gTaskManager.SwitchTo(gpTopMenuTask);
    } else if (event.source == MICROBIT_ID_TIMER) {
        m_image = kMotoBase;
        if (event.value & 0x08) {
          m_image &= ~(0x04 << 10);
        }
    }
}
