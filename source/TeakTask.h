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


// Packed Bitmap - a 5x5 micor:bit bit map packed into an int32
// the fraem count is used in some context to indicate how many
// times to repeat that frame.
//
//  0|-fc|--row5--|--row4--|--row3--|--row2--|--row2--|

#define PBMAP_ROW(_b1, _b2, _b3, _b4, _b5) \
  (((_b1 & 1)<<4) | ((_b2&1)<<3) | ((_b3&1)<<2) | ((_b4&1)<<1) | ((_b5&1)<<0))

#define PBMAP_FRAME_COUNT(_fc)   (_fc & 0x1F)

#define PBMAP(_r1, _r2, _r3, _r4, _r5, frameCount) \
  (_r1 | (_r2<<5) | (_r3<<10) | (_r4<<15) | (_r5<<20) | (frameCount<<25))

inline int PBmapFrameCount(int pbmap) { return (pbmap & (0x1F << 25)) >> 25;}
int PBmapUnpack(int pbmap, uint8_t* bits, int width);

// Adding a new event source
#define MICROBIT_ID_TIMER 512

// Case # 21-05-398

// This order must match the array initializer at the end of TeakTasks.cpp
enum TaskId : uint8_t {
    kSameTask = 0,
    kBootTask,
    kTopMenuTask,
    kScrollTask,
    kFirstInRing, kMotorTask = kFirstInRing,
    kUserProgramTask,
    kBlueToothTask,
    kLevelTask,
    kTempTask,  kLastInRing = kTempTask
};

class TeakTask {
public:
    int m_image;
public:
    int PackedImage() { return m_image; };
    virtual TaskId Event(MicroBitEvent) { return kSameTask; };
};
extern TeakTask* gTasks[];

// TeakMenu - routs events to the appropriate module
class TeakTaskManager {
public:
    TeakTaskManager();
    void Event(MicroBitEvent event);
    TeakTask* CurrentTask() { return gTasks[m_currentTask]; };
private:
    int m_currentImage = 0;
    TaskId m_currentTask;
};
extern TeakTaskManager gTaskManager;
