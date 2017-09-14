/*
Copyright (c) 2017 Paul Austin.

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
#include "TeakTask.h"

extern MicroBit uBit;

/*
Menu state machines.

up power up. the bot goes though it laucnh picture them into the menu top.
at that point the A and B button scroll left/tight through top optiopns.
There wont be many 2-5 options. Its a curculare list so the user can get to any
in n/2 button taps.

1. Motor
2. P1, P2  - user programs
3. Status  - low level daignostics TBD
4. Pairing mode.

*/

// Images are unpacked or merged in one buffer before
// being transfered to the main display.
extern TeakTask *gTasks[];
TeakTaskManager gTaskManager;

TeakTaskManager::TeakTaskManager()
{
    // Start whth the boot task.
    m_currentTask = kBootTask;
}

void TeakTaskManager::Event(MicroBitEvent event)
{
    TaskId nextTask = CurrentTask()->Event(event);

    int newImage = CurrentTask()->PackedImage();
    if (newImage != m_currentImage)  {
        PBmapUnpack(newImage, uBit.display.image.getBitmap(), uBit.display.image.getWidth());
        m_currentImage = newImage;
    }
    if (nextTask != kSameTask) {
        m_currentTask = nextTask;
    }
}

int PBmapUnpack(int pbmap, uint8_t* bytes, int width)
{
    int bits = pbmap;
    uint8_t* rowBytes = bytes;
    for (int i=0; i < 5; i++) {
        rowBytes[0] = (bits & (0x01 << 4));
        rowBytes[1] = (bits & (0x01 << 3));
        rowBytes[2] = (bits & (0x01 << 2));
        rowBytes[3] = (bits & (0x01 << 1));
        rowBytes[4] = (bits & (0x01 << 0));
        rowBytes += width;
        bits = bits >> 5;
    }
    return 0;
}

#if 0
// Just for reference
#define MICROBIT_BUTTON_EVT_DOWN                1
#define MICROBIT_BUTTON_EVT_UP                  2
#define MICROBIT_BUTTON_EVT_CLICK               3
#define MICROBIT_BUTTON_EVT_LONG_CLICK          4
#define MICROBIT_BUTTON_EVT_HOLD                5
#define MICROBIT_BUTTON_EVT_DOUBLE_CLICK        6

#define MICROBIT_BUTTON_LONG_CLICK_TIME         1000
#define MICROBIT_BUTTON_HOLD_TIME               1500

#define MICROBIT_BUTTON_STATE                   1
#define MICROBIT_BUTTON_STATE_HOLD_TRIGGERED    2
#define MICROBIT_BUTTON_STATE_CLICK             4
#define MICROBIT_BUTTON_STATE_LONG_CLICK        8
#endif

//------------------------------------------------------------------------------
// The initial task that starts when the BM boots.
class BootTask : public TeakTask {
public:
    TaskId Event(MicroBitEvent event);
private:
    uint8_t m_frame;
};

BootTask gBootTask;

const int bootImages[] __attribute__ ((aligned(4))) = {
  PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(4)),
  PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(4)),
  PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(1, 1, 0, 1, 1),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(4)),
  PBMAP(
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_FRAME_COUNT(4)),
  PBMAP(
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_FRAME_COUNT(4)),
    0 // End of film strip
  };

TaskId BootTask::Event(MicroBitEvent event)
{
    if (event.source == MICROBIT_ID_TIMER) {
      m_image = bootImages[m_frame];
      m_frame++;
    }

    return m_frame < 5 ? kSameTask : kTopMenuTask;
}

//------------------------------------------------------------------------------
// A mini task for scrolling from one item to another.
class ScrollTask : public TeakTask {
public:
    TaskId Setup(TaskId current, bool toLeft);
    TaskId Event(MicroBitEvent event);
public:
    TaskId  m_next;
    uint8_t m_step;
    uint8_t m_toLeft;
};
ScrollTask gScrollTask;

TaskId ScrollTask::Setup(TaskId current, bool toLeft)
{
    m_image = gTasks[current]->PackedImage();
    if (toLeft) {
        if (current >= kLastInRing) {
            m_next = kFirstInRing;
        } else {
            m_next = (TaskId) (current + 1);
        }
    } else {
        if (current <= kFirstInRing) {
          m_next = kLastInRing;
        } else {
          m_next = (TaskId) (current - 1);
        }
    }
    m_step = 0;
    m_toLeft = toLeft;
    return m_next;
}

TaskId ScrollTask::Event(MicroBitEvent event)
{
    if (event.source != MICROBIT_ID_TIMER)
        return kSameTask;

//    int imageFrom = gTasks[m_from]->PackedImage();
    int imageTo = gTasks[m_next]->PackedImage();

    int leftMask = PBMAP(
        PBMAP_ROW(1, 0, 0, 0, 0), PBMAP_ROW(1, 0, 0, 0, 0),
        PBMAP_ROW(1, 0, 0, 0, 0), PBMAP_ROW(1, 0, 0, 0, 0),
        PBMAP_ROW(1, 0, 0, 0, 0),
        0);

    int rightMask = PBMAP(
        PBMAP_ROW(0, 0, 0, 0, 1), PBMAP_ROW(0, 0, 0, 0, 1),
        PBMAP_ROW(0, 0, 0, 0, 1), PBMAP_ROW(0, 0, 0, 0, 1),
        PBMAP_ROW(0, 0, 0, 0, 1),
        0);

    // In each frame slide From and replace right/left edge with
    // bits from To frame.
    if (m_toLeft) {
        m_image = (m_image << 1) & ~rightMask;
        if (m_step>0) {
            m_image = m_image | ((imageTo >> (5-m_step)) & rightMask);
        }
    } else {
        m_image = (m_image >> 1) & ~leftMask;
        if (m_step>0) {
            m_image = m_image | ((imageTo << (5-m_step)) & leftMask);
        }
    }

    // Advance the animation frame
    m_step++;

    return (m_step < 5) ? kSameTask : kTopMenuTask ;
}

//------------------------------------------------------------------------------
class TopMenuTask : public TeakTask {
public:
    TopMenuTask();
    TaskId Event(MicroBitEvent event);
    TeakTask* ActiveTask() { return gTasks[m_activeTask]; }
public:
    TaskId m_activeTask;
};
TopMenuTask gTopMenuTask;

TopMenuTask::TopMenuTask()
{
    m_activeTask = kFirstInRing;
    m_image = ActiveTask()->PackedImage();
}

TaskId TopMenuTask::Event(MicroBitEvent event)
{
    if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
        if (event.source == MICROBIT_ID_BUTTON_A) {
            m_activeTask = gScrollTask.Setup(m_activeTask, true);
            return kScrollTask;
        } else if (event.source == MICROBIT_ID_BUTTON_B) {
            m_activeTask = gScrollTask.Setup(m_activeTask, false);
            return kScrollTask;
        }
    } else if (event.value == MICROBIT_BUTTON_EVT_HOLD) {
        if (event.source == MICROBIT_ID_BUTTON_AB) {
            // activeate the advanced mode for the task.
            uBit.display.print('H');
        }
    } else if (event.source == MICROBIT_ID_TIMER) {
      //  ActiveTask()->Event(event);
        m_image = ActiveTask()->PackedImage();
    }
    // else if timer then send it to highlighte task
    return kSameTask;
}

//------------------------------------------------------------------------------
// A task for direct control of the motors
class MotorTask : public TeakTask {
public:
    MotorTask();
    TaskId Event(MicroBitEvent event);
};
MotorTask gMotorTask;

MotorTask::MotorTask()
{
    m_image =  PBMAP(
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_ROW(0, 1, 0, 1, 0),
        PBMAP_ROW(1, 1, 1, 1, 1),
        PBMAP_ROW(0, 1, 0, 1, 0),
        PBMAP_FRAME_COUNT(1));
}

TaskId MotorTask::Event(MicroBitEvent event)
{
    if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
        if (event.source == MICROBIT_ID_BUTTON_A) {
            uBit.display.print('A');
        } else if (event.source == MICROBIT_ID_BUTTON_B) {
            uBit.display.print('B');
        }
    }
    return kSameTask;
}

//------------------------------------------------------------------------------
// A task for running the users saved program.
// Perhaps there are 2 or 3 programs
class UserProgramTask : public TeakTask {
public:
    UserProgramTask();
};
UserProgramTask gUserProgramTask;

UserProgramTask::UserProgramTask()
{
    m_image = PBMAP(
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(1, 0, 0, 0, 1),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_FRAME_COUNT(1));
}

//------------------------------------------------------------------------------
// A task for running the users saved program.
// Perhaps there are 2 or 3 programs
class BlueToothTask : public TeakTask {
public:
    BlueToothTask();
};
BlueToothTask gBlueToothTask;

BlueToothTask::BlueToothTask()
{
    m_image = PBMAP(
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_FRAME_COUNT(1));
}

//------------------------------------------------------------------------------
// A teask to use the built-in level (accelerometer)
class LevelTask  : public TeakTask {
public:
    LevelTask();
};
LevelTask gLevelTask;

LevelTask::LevelTask()
{
    m_image = PBMAP(
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 1, 0, 1, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_FRAME_COUNT(1));
}

//------------------------------------------------------------------------------
// A teask to use the built-in temp
class TempTask  : public TeakTask {
public:
    TempTask();
};
TempTask gTempTask;

TempTask::TempTask()
{
    m_image = PBMAP(
        PBMAP_ROW(0, 1, 1, 1, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_FRAME_COUNT(1));
}
//------------------------------------------------------------------------------
// Set up the intial task to be the boot task, this will
// run the startup screen

TeakTask *gTasks[] = {
    (TeakTask*) NULL,
    &gBootTask,
    &gTopMenuTask,
    &gScrollTask,
    &gMotorTask,
    &gUserProgramTask,
    &gBlueToothTask,
    &gLevelTask,
    &gTempTask
};
