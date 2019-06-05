/*
Copyright (c) 2019 Trashbots.

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

//------------------------------------------------------------------------------
// MotorTask - A task for direct control of the motors
class EmojiTask : public TeakTask {
public:
    EmojiTask();
    void Event(MicroBitEvent event);
};
EmojiTask gEmojiTask;
TeakTask* gpEmojiTask = &gEmojiTask;

extern MicroBit uBit;

const int kEmojiSmile = PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_FRAME_COUNT(1));

const int kEmojiFrown = PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_ROW(1, 0, 0, 0, 1),
    PBMAP_FRAME_COUNT(1));

const int kEmojiBored = PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(1, 1, 1, 1, 1),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(1));

const int kEmojiWow = PBMAP(
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 1, 1, 1, 0),
    PBMAP_FRAME_COUNT(1));

const int kEmojiTired = PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(1, 1, 0, 1, 1),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(1, 1, 1, 1, 1),
    PBMAP_FRAME_COUNT(1));

const int kEmojiCrazy = PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(1, 0, 1, 0, 1),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_FRAME_COUNT(1));

const int kEmojiReaction[] = {kEmojiSmile, kEmojiTired, kEmojiFrown, kEmojiCrazy, kEmojiBored, kEmojiWow};
const int notes[] = {ksNoteA5, ksNoteC4, ksNoteD4, ksNoteE4, ksNoteF4, ksNoteG4};
int spot;

EmojiTask::EmojiTask()
{
    m_note = ksNoteF4;
    m_image = kEmojiSmile;
}

void EmojiTask::Event(MicroBitEvent event)
{
    if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
        m_image = kEmojiBored;
        if (event.source == MICROBIT_ID_BUTTON_B) {
          // pick the next spot in the array
          if (spot < 5){
            spot++;
          } else {
            spot = 0;
          }

          PlayNote(notes[spot], 64);
          m_image = kEmojiReaction[spot];

        } else if (event.source == MICROBIT_ID_BUTTON_A) {
          // pick the previous spot in the array
          if (spot > 0){
            spot--;
          } else {
            spot = 5;
          }

          PlayNote(notes[spot], 64);
          m_image = kEmojiReaction[spot];

        } else if (event.source == MICROBIT_ID_BUTTON_AB) {
        }
    } else if (event.value == MICROBIT_BUTTON_EVT_HOLD) {
      if (event.source == MICROBIT_ID_BUTTON_B) {
        gTaskManager.SwitchTo(gpMotorTask);
      } else if (event.source == MICROBIT_ID_BUTTON_A) {
        uBit.display.scrollAsync(gTaskManager.BotName(), 70);
        PlayNoteStream(ksNoteC4);
        PlayNoteStream(ksNoteD4);
        PlayNoteStream(ksNoteE4);
        PlayNoteStream(ksNoteF4);
        PlayNoteStream(ksNoteG4);
        PlayNoteStream(ksNoteA5);
        PlayNoteStream(ksNoteB5);
        PlayNoteStream(ksNoteC5);
      }
    } else if (event.source == MICROBIT_ID_TIMER) {
    }
}
