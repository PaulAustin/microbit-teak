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

extern MicroBit uBit;

const int bootImages[] __attribute__ ((aligned(4))) = {
  PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(3)),
  PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(2)),
  PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(1, 1, 1, 1, 1),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(2)),
  PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(1, 1, 0, 1, 1),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(2)),
  PBMAP(
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 1, 0, 0),
    PBMAP_FRAME_COUNT(1)),
  PBMAP(
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_FRAME_COUNT(1)),
  PBMAP(
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_FRAME_COUNT(1)),
  PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(5)),
    0 // End of film strip
  };

//------------------------------------------------------------------------------
// BootTask - Manage boot up animations sounds.
// The initial task that starts when the BM boots.
class BootTask : public TeakTask {
public:
    BootTask();
    void Event(MicroBitEvent event);
private:
    uint8_t m_frame;
    uint8_t m_delay;
    enum {
      kTaskIdle = 0,
      kTaskSplashAnimation,
      kTaskSplashDone,
      kTaskScollBotName
    };
};

BootTask gBootTask;
TeakTask *gpBootTask = &gBootTask;

BootTask::BootTask()
{
    m_frame = 0;
    m_delay = PBmapFrameCount(bootImages[m_frame]);
    m_state = kTaskSplashAnimation;
}

void BootTask::Event(MicroBitEvent event)
{
    if (event.source == MICROBIT_ID_TIMER) {
        if (m_state == kTaskSplashAnimation) {
            // True while the film is playing.
            m_image = bootImages[m_frame];
            m_delay--;
            if (m_delay <= 0) {
                // Bump to next frame, and reset delay
                m_frame++;
                m_delay = PBmapFrameCount(bootImages[m_frame]);
            }
            if (m_delay == 0) {
                m_state = kTaskSplashDone;
            }
        } else if (m_state == kTaskSplashDone) {
            // Scolling is done in background of of the main loop.
            // Event wil fire once scrolling is complete.
            uBit.display.scrollAsync(gTaskManager.BotName(), 70);
            m_state = kTaskScollBotName;
        }
    } else if (event.source == MICROBIT_ID_DISPLAY) {
        // MICROBIT_DISPLAY_EVT_FREE is fired once the name has scrolled by.
        // Nothing left to do.
        m_state = kTaskIdle;
        gTaskManager.SwitchTo(gpTopMenuTask);
    } else if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
        // A button tap skips to main screen
        uBit.display.stopAnimation();
        m_state = kTaskIdle;
        gTaskManager.SwitchTo(gpTopMenuTask);
    }
}
