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

//------------------------------------------------------------------------------
// BlueToothTask - A task to show more details on the connection, and to allow
// configure basic settings (e.g. on/off)

class BlueToothTask : public TeakTask {
public:
    BlueToothTask();
    bool m_advertising;
    void Event(MicroBitEvent event);
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

void BlueToothTask::Event(MicroBitEvent event)
{

#if 0
if (m_booting == kSplashDone) {
    // Scolling is done in background of
    // of the main loop.
    uBit.display.scrollAsync(gTaskManager.m_name, 80);
    m_booting = kBotNameScrolling;
}
} else if (event.source == MICROBIT_ID_DISPLAY) {
// MICROBIT_DISPLAY_EVT_FREE is fired once the name has scrolled by.
m_booting = kBootDone;
#endif

    if (event.value == MICROBIT_BUTTON_EVT_HOLD &&
        event.source == MICROBIT_ID_BUTTON_AB) {
        // Turn off the beacon
        m_advertising = false;
        setAdvertising(m_advertising);
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
}
