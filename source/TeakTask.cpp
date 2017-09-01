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


void TeakTaskManager::MBEventA(MicroBitEvent)
{}
void TeakTaskManager::MBEventB(MicroBitEvent)
{}
void TeakTaskManager::MBEventAB(MicroBitEvent)
{}
void TeakTaskManager::BackgroundTick()
{}

//------------------------------------------------------------------------------
// The initial task that starts when the BM boots.
class MBBootTask : public TeakTask {
//  virtual void Show(int frame);
//  virtual void Activate();
//  virtual void MB_Event(MicroBitEvent mbEvt);
};
MBBootTask gBootTask;

//------------------------------------------------------------------------------
// A mini task for scrolling from one item to another.
class MenuScrollTask : public TeakTask {
  virtual void Show(int frame);
//  virtual void Activate();
//  virtual void MB_Event(MicroBitEvent mbEvt);
};
MenuScrollTask gScrollTask;

//------------------------------------------------------------------------------
// A task for direct control of the motors
class MBMotorTask : public TeakTask {
  virtual void Show(int frame);
//  virtual void Activate();
//  virtual void MB_Event(MicroBitEvent mbEvt);
};
MBMotorTask gMotorTask;

//------------------------------------------------------------------------------
// A task for running the users saved program.
// Perhaps there are 2 or 3 programs
class MBUserProgramTask : public TeakTask {
//  virtual void Show(int frame);
//  virtual void Activate();
//  virtual void MB_Event(MicroBitEvent mbEvt);
};
MBUserProgramTask gUserProgramTask;

//------------------------------------------------------------------------------
// A teask to use the built-in level (accelerometer)
class MBLevelTask  : public TeakTask {
//  virtual void Show(int frame) = 0;
//  virtual void Activate() = 0;
//  virtual void MB_Event(MicroBitEvent mbEvt) = 0;
};
MBLevelTask gLevelTask;

//------------------------------------------------------------------------------
// A teask to use the built-in temp
class MBTempTask  : public TeakTask {
//  virtual void Show(int frame) = 0;
//  virtual void Activate() = 0;
//  virtual void MB_Event(MicroBitEvent mbEvt) = 0;
};
MBTempTask gTempTask;

void MenuScrollTask::Show(int frame) {
  if (frame < 5) {
    // Merge the image for previous and next module
  } else {
    // Switch to the next module.
  }
}

char MBMotorModeGlyph [] {
  MB_PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  MB_PACK_DISPLAY_BITS(0, 0, 0, 0, 0),
  MB_PACK_DISPLAY_BITS(0, 1, 0, 1, 0),
  MB_PACK_DISPLAY_BITS(1, 1, 1, 1, 1),
  MB_PACK_DISPLAY_BITS(0, 1, 0, 1, 0),
};

void MBMotorTask::Show(int frame)
{
    if (frame == 0) {
      //ShowImage(MBMotorModeGlyph);
    }
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
