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

#if 1
bool hackConnected = false;
const int pm_connect = PBMAP(
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_FRAME_COUNT(1));

const int pm_disconnect = PBMAP(
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_FRAME_COUNT(1));

#endif

TaskId CheckBLEEvents(MicroBitEvent event);

TeakTaskManager::TeakTaskManager()
{
    // Start wth the boot task.
    m_currentTask = kBootTask;
}

void TeakTaskManager::Event(MicroBitEvent event)
{
    TaskId next = CheckBLEEvents(event);
    if (next != kSameTask) {
        return;
    }

    if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
         if (event.source == MICROBIT_ID_BUTTON_A) {
            // uBit.display.print('A');
             uart->send(ManagedString("(a)"));
         } else if (event.source == MICROBIT_ID_BUTTON_B) {
            // uBit.display.print('B');
             uart->send(ManagedString("(b)"));
         } else if (event.source == MICROBIT_ID_BUTTON_AB) {
            // uBit.display.print('AB');
             uart->send(ManagedString("(ab)"));
         }
    }

    // If connected then ignore local menu system.
    if (hackConnected)
        return;

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
// Turn on bluetooth advertising.
class BlueToothTask : public TeakTask {
public:
    BlueToothTask();
    bool m_advertising;
    TaskId Event(MicroBitEvent event);
};
BlueToothTask gBlueToothTask;
TeakTask *gpBlueToothTask = &gBlueToothTask;

const int kBluetootBaseImage = PBMAP(
    PBMAP_ROW(1, 1, 1, 1, 1),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_FRAME_COUNT(1));

BlueToothTask::BlueToothTask()
{
    m_image = kBluetootBaseImage;
}

TaskId BlueToothTask::Event(MicroBitEvent event)
{
    if (event.value == MICROBIT_BUTTON_EVT_HOLD &&
        event.source == MICROBIT_ID_BUTTON_AB) {
        // Turn off the beacon
        m_advertising = false;
        setAdvertising(m_advertising);
        return kTopMenuTask;
    } else if (event.source == MICROBIT_ID_TIMER) {
        if (!m_advertising) {
            m_advertising = true;
            setAdvertising(m_advertising);
        }
        m_image = kBluetootBaseImage;
        if (event.value & 0x08) {
            m_image &= ~(0x04 << 10);
        }
        // Once connected to it can pop back to the top menu.
    }
    return kSameTask;
}

//------------------------------------------------------------------------------
TaskId CheckBLEEvents(MicroBitEvent event)
{
    // Watch for events that are managed independent of state.
    if (event.source == MICROBIT_ID_BLE) {
        if (event.value == MICROBIT_BLE_EVT_CONNECTED) {
            hackConnected = true;
            gBlueToothTask.m_image = pm_connect;
            uBit.display.print('C');
            return kUserProgramTask;
        } else if (event.value == MICROBIT_BLE_EVT_DISCONNECTED) {
            hackConnected = false;
            gBlueToothTask.m_image = pm_disconnect;
            uBit.display.print('D');
            return kBlueToothTask;
        }
    }
    return kSameTask;
};

//------------------------------------------------------------------------------
// Set up the intial task to be the boot task, this will
// run the startup screen

extern TeakTask* gpBootTask;
extern TeakTask* gpTopMenuTask;
extern TeakTask* gpScrollTask;
extern TeakTask* gpMotorTask;
extern TeakTask* gpUserProgramTask;
extern TeakTask* gpLevelTask;
extern TeakTask* gpTempTask;

TeakTask *gTasks[] = {
    (TeakTask*) NULL,
    gpBootTask,
    gpTopMenuTask,
    gpScrollTask,
    gpMotorTask,
    gpUserProgramTask,
    gpBlueToothTask,
    gpLevelTask,
    gpTempTask
};

//------------------------------------------------------------------------------
void setAdvertising(bool state)
{
    if (state) {
        uBit.bleManager.setTransmitPower(6);
        uBit.bleManager.ble->setAdvertisingInterval(200);
        uBit.bleManager.ble->gap().setAdvertisingTimeout(0);
        uBit.bleManager.ble->gap().startAdvertising();
    } else {
        uBit.bleManager.stopAdvertising();
    }
}
