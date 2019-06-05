/*
Copyright (c) 2019 Trashbots, Inc. - SDG

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
// TopMenuTask - Manages the top activity menu that can scroll left ot right
class TopMenuTask : public TeakTask {
public:
    TopMenuTask();
    void Event(MicroBitEvent event);
    void SetupScroll(bool toLeft);
    void Launch();
    void Scroll();
    void SwipeIn();
    void OpenTop();
    enum {
      kTopMenuIdle,
      kTopMenuScrollLeft,
      kTopMenuScrollRight,
      kTopMenuSwipeIn,
      kTopMenuSwipeOut,
    };
private:
    int m_state;
    int m_frame;
    TeakTask* m_focusTask;
    TeakTask* m_nextTask;
};
TopMenuTask gTopMenuTask;
TeakTask* gpTopMenuTask = &gTopMenuTask;

//------------------------------------------------------------------------------
TopMenuTask::TopMenuTask()
{
    m_state = kTopMenuIdle;
    m_focusTask = gpEmojiTask;
    m_image = m_focusTask->PackedImage();
}

//------------------------------------------------------------------------------
void TopMenuTask::SetupScroll(bool toLeft)
{
    m_frame = 0;

    TeakTask *t = m_focusTask;
    if (m_state == kTopMenuScrollLeft || m_state == kTopMenuScrollRight) {
        // If in the midst of scrolling
        // Assume the first one is finished and start from there.
        t = m_nextTask;
    }

    if (toLeft) {
        m_state = kTopMenuScrollLeft;
        m_nextTask = t->m_leftTask;
    } else {
        m_state = kTopMenuScrollRight;
        m_nextTask = t->m_rightTask;
    }

    PlayNote(m_nextTask->m_note, 64);
}

//------------------------------------------------------------------------------
void TopMenuTask::Scroll()
{
    int imageTo = m_nextTask->PackedImage();

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
    if (m_state == kTopMenuScrollLeft) {
        m_image = (m_image << 1) & ~rightMask;
        if (m_frame > 0) {
            m_image = m_image | ((imageTo >> (5-m_frame)) & rightMask);
        }
    } else {
        m_image = (m_image >> 1) & ~leftMask;
        if (m_frame > 0) {
            m_image = m_image | ((imageTo << (5-m_frame)) & leftMask);
        }
    }

    // Advance the animation frame, pop to top when done.
    m_frame++;
    if (m_frame > 5) {
        m_state = kTopMenuIdle;
        m_frame = 0;
        m_focusTask = m_nextTask;
        m_nextTask = NULL;
    }
}

//------------------------------------------------------------------------------
void TopMenuTask::Launch()
{
    m_state = kTopMenuSwipeIn;
    m_frame = 0;
}

//------------------------------------------------------------------------------
void TopMenuTask::OpenTop()
{
    m_state = kTopMenuSwipeOut;
    m_frame = 0;
}
//------------------------------------------------------------------------------
void TopMenuTask::SwipeIn()
{
    // Animate bar from top or bottom based on state.
    m_image = m_focusTask->PackedImage();
    if (m_state == kTopMenuSwipeIn) {
        m_image = m_image | 0x0000001f << (5 * m_frame);
    } else {
        m_image = m_image | (0x0000001f << 20) >> (5 * m_frame);
    }

    m_frame++;
    if (m_frame > 5) {
        m_frame = 0;
        if (m_state == kTopMenuSwipeIn) {
            m_state = kTopMenuIdle;
            gTaskManager.SwitchTo(m_focusTask);
        }
    }
}

void TopMenuTask::Event(MicroBitEvent event)
{
    if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
        // Simple Button Down
        if (event.source == MICROBIT_ID_BUTTON_A) {
            // PlayNote(40, 32);
            this->SetupScroll(false);
        } else if (event.source == MICROBIT_ID_BUTTON_B) {
            // PlayNote(42, 32);
            this->SetupScroll(true);
        }
    } else if (event.source == MICROBIT_ID_TASK_SWAP ) {
        // Right Button Hold to launch
        Launch();
    } else if (event.source == MICROBIT_ID_TIMER) {
        // Right Button Hold to launch
        switch(m_state) {
            case kTopMenuSwipeIn:
                SwipeIn();
                break;
            case kTopMenuScrollLeft:
            case kTopMenuScrollRight:
                Scroll();
                break;
        }
    }
}
