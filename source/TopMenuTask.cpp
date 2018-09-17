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

//------------------------------------------------------------------------------
// ScrollTask - A mini task for scrolling from one item to another left or right
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
TeakTask* gpScrollTask = &gScrollTask;

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
// TopMenuTask - Manages the top activity menu that can scroll left ot rigth
class TopMenuTask : public TeakTask {
public:
    TopMenuTask();
    TaskId Event(MicroBitEvent event);
    TeakTask* ActiveTask() { return gTasks[m_activeTask]; }
    TaskId HighlightLine();
    enum {
      kTopMenuBrowse,
      kTopMenuSwipeIn,
      kTopMenuSwipeOut,
    };
public:
    TaskId m_activeTask;
    int8_t m_highlightFrame;
    int8_t m_state;
};
TopMenuTask gTopMenuTask;
TopMenuTask* gpTopMenuTask = &gTopMenuTask;

TopMenuTask::TopMenuTask()
{
    m_activeTask = kFirstInRing;
    m_state = kTopMenuBrowse;
    m_image = ActiveTask()->PackedImage();
}

TaskId TopMenuTask::HighlightLine()
{
    m_image = ActiveTask()->PackedImage();
    // Animate bar from top or bottom based on state.
    int offset = (m_state == kTopMenuSwipeIn)
        ? m_highlightFrame
        : (4 - m_highlightFrame);
    int mask = 0x0000001f << (5 * offset);
    m_image = m_image | mask;

    // Prep for next frame.
    if (m_highlightFrame < 5) {
        // Still animatting,
        m_highlightFrame++;
        return kSameTask;
    } else {
        // All done, reset the state and frame count.
        m_highlightFrame = 0;
        if (m_state == kTopMenuSwipeIn) {
            // Swipped in the new task, activte it
            // and leave this teask ready to swipe back in.
            m_state = kTopMenuSwipeOut;
            return m_activeTask;
        } else {
            // The old task has been swipped out , change the state
            // and stay with the menu task.
            m_state = kTopMenuBrowse;
            return kSameTask;
        }
    }
}

TaskId TopMenuTask::Event(MicroBitEvent event)
{
    // Watch for events that are managed independent of state.
    // process events that are specific to state.
    switch(m_state) {
    case kTopMenuSwipeIn:
    case kTopMenuSwipeOut:
        if (event.source == MICROBIT_ID_TIMER) {
          return HighlightLine();
        }
        break;
    case kTopMenuBrowse:
        if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
            m_state = kTopMenuBrowse;
            if (event.source == MICROBIT_ID_BUTTON_A) {
                m_activeTask = gScrollTask.Setup(m_activeTask, false);
                return kScrollTask;
            } else if (event.source == MICROBIT_ID_BUTTON_B) {
                m_activeTask = gScrollTask.Setup(m_activeTask, true);
                return kScrollTask;
            }
        } else if (event.value == MICROBIT_BUTTON_EVT_HOLD) {
            if (event.source == MICROBIT_ID_BUTTON_AB) {
                m_state = kTopMenuSwipeIn;
                return kSameTask;
            }
        } else if (event.source == MICROBIT_ID_TIMER) {
            m_image = ActiveTask()->PackedImage();
        }
        break;
    }
    return kSameTask;
}
