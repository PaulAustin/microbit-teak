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
extern ScrollTask* gpScrollTask;

//------------------------------------------------------------------------------
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
                m_activeTask = gpScrollTask->Setup(m_activeTask, false);
                return kScrollTask;
            } else if (event.source == MICROBIT_ID_BUTTON_B) {
                m_activeTask = gpScrollTask->Setup(m_activeTask, true);
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
