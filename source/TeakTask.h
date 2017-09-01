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

// A macro that make sit easy to make a small packed bitmaps
// for the 5x5 display while leaving it 'possible' to read the
// pattern instead of hex digits.
#define PACK_DISPLAY_BITS(_b1, _b2, _b3, _b4, _b5) \
  ((_b1<<7) | (_b2<<6) | (_b3<<5) | (_b4<<4) | (_b5<<3))

class TeakTask;

// TeakMenu - routs events to the appropriate module
class TeakTaskManager {
  public:
    static void SetTask(TeakTask* pTask);
    static void MBEventA(MicroBitEvent mbEvt);
    static void MBEventB(MicroBitEvent mbEvt);
    static void MBEventAB(MicroBitEvent mbEvt);
    static void Tick();
  private:
    static TeakTask* m_currentTask;
    static int       m_frameState;
};

class TeakTask {
  public:
    virtual int Tick(int state) { return state; };
    virtual void MB_Event(MicroBitEvent) {};
};
