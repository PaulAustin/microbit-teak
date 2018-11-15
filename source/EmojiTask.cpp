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
// MotorTask - A task for direct control of the motors
class EmojiTask : public TeakTask {
public:
    EmojiTask();
    void Event(MicroBitEvent event);
};
EmojiTask gEmojiTask;
TeakTask* gpEmojiTask = &gEmojiTask;

const int kEmojiSmile = PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_FRAME_COUNT(1));

EmojiTask::EmojiTask()
{
    m_image = kEmojiSmile;
}

void EmojiTask::Event(MicroBitEvent event)
{
    if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
        m_image = kEmojiSmile;
        if (event.source == MICROBIT_ID_BUTTON_A) {
        } else if (event.source == MICROBIT_ID_BUTTON_B) {
        } else if (event.source == MICROBIT_ID_BUTTON_AB) {
        }
    } else if (event.value == MICROBIT_BUTTON_EVT_HOLD) {
    } else if (event.source == MICROBIT_ID_TIMER) {
    }
}
