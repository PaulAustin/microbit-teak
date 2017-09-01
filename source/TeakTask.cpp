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

/*
uint8_t *getBitmap()
{
    return ptr->data;
}

*/

// Images are unpacked or merged in one buffer before
// being transfered to the main display.
MicroBitImage g_menuImage;

void DisplayPackedBits(char* packedBits)
{
  // Blast bits into the display.
  uint8_t* bits = uBit.display.image.getBitmap();
  int width = uBit.display.image.getWidth();
  for(int y = 0; y < 5; y++) {
    uint8_t* pByte = &bits[y * width];
    int packedRow = packedBits[y];
    *pByte = (packedRow & (0x01 << 7)) ? 1 : 0;       pByte++;
    *pByte = (packedRow & (0x01 << 6)) ? 1 : 0;       pByte++;
    *pByte = (packedRow & (0x01 << 5)) ? 1 : 0;       pByte++;
    *pByte = (packedRow & (0x01 << 4)) ? 1 : 0;       pByte++;
    *pByte = (packedRow & (0x01 << 3)) ? 1 : 0;
  }
}

void TeakTaskManager::SetTask(TeakTask* pTask)
{
  if (m_currentTask) {
    // message the task that its swapped out
  }
  m_currentTask = pTask;
  if (m_currentTask) {
    m_frameState = m_currentTask->Tick(m_frameState);
  }
}
void TeakTaskManager::MBEventA(MicroBitEvent event)
{
  m_currentTask->MB_Event(event);
}
void TeakTaskManager::MBEventB(MicroBitEvent event)
{
  m_currentTask->MB_Event(event);
}
void TeakTaskManager::MBEventAB(MicroBitEvent event)
{
  m_currentTask->MB_Event(event);
}
void TeakTaskManager::Tick()
{
  m_frameState = m_currentTask->Tick(m_frameState);
}

//------------------------------------------------------------------------------
// The initial task that starts when the BM boots.
class BootTask : public TeakTask {
  public:
    virtual int Tick(int frame);
};

char BootFilm [] {
  // five bytes per frame
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(0, 1, 0, 1, 0),
  PACK_DISPLAY_BITS(1, 1, 1, 1, 1),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  //-----------------------------
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(1, 1, 1, 1, 1),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  //-----------------------------
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(1, 1, 1, 1, 1),
  PACK_DISPLAY_BITS(0, 1, 0, 1, 0),
  //-----------------------------
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(1, 1, 1, 1, 1),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
};

int BootTask::Tick(int state) {
  DisplayPackedBits(BootFilm + ((state % 4) * 5));
  return state + 1;
}
BootTask gBootTask;

//------------------------------------------------------------------------------
// A mini task for scrolling from one item to another.
class MenuScrollTask : public TeakTask {
  public:
    virtual int Tick(int frame);
};
MenuScrollTask gScrollTask;

//------------------------------------------------------------------------------
// A task for direct control of the motors
class MotorTask : public TeakTask {
  public:
    virtual int Tick(int frame);
//  virtual void Activate();
//  virtual void MB_Event(MicroBitEvent mbEvt);
};
MotorTask gMotorTask;

//------------------------------------------------------------------------------
// A task for running the users saved program.
// Perhaps there are 2 or 3 programs
class UserProgramTask : public TeakTask {
//  virtual void Show(int frame);
//  virtual void Activate();
//  virtual void MB_Event(MicroBitEvent mbEvt);
};
UserProgramTask gUserProgramTask;

//------------------------------------------------------------------------------
// A teask to use the built-in level (accelerometer)
class LevelTask  : public TeakTask {
//  virtual void Show(int frame) = 0;
//  virtual void Activate() = 0;
//  virtual void MB_Event(MicroBitEvent mbEvt) = 0;
};
LevelTask gLevelTask;

//------------------------------------------------------------------------------
// A teask to use the built-in temp
class TempTask  : public TeakTask {
//  virtual void Show(int frame) = 0;
//  virtual void Activate() = 0;
//  virtual void MB_Event(MicroBitEvent mbEvt) = 0;
};
TempTask gTempTask;

int MenuScrollTask::Tick(int frame) {
  if (frame < 5) {
    return frame + 1;
    // Merge the image for previous and next module
  } else {
    // Switch to the next module.
    return 0;
  }

}

char MBMotorModeGlyph [] {
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  PACK_DISPLAY_BITS(0, 1, 0, 1, 0),
  PACK_DISPLAY_BITS(1, 1, 1, 1, 1),
  PACK_DISPLAY_BITS(0, 1, 0, 1, 0),
};

int MotorTask::Tick(int frame)
{
    if (frame == 0) {
      //ShowImage(MBMotorModeGlyph);
    }
    return frame + 1;
}

/*
  if (evt.value == MICROBIT_BUTTON_EVT_HOLD) {
    bAdvertising = !bAdvertising;
    setAdvertising(bAdvertising);
  }
*/

//------------------------------------------------------------------------------
// Set up the intial task to be the boot task, this will
// run the startup screen

TeakTask* TeakTaskManager::m_currentTask = &gBootTask;
int TeakTaskManager::m_frameState = 0;
