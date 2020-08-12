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
#include "TBCDriver.h"
#include "teak/tstring.h"

extern MicroBit uBit;
//SPI spi(MOSI, MISO, SCK);

void TBCInit()
{
  //spi.format(8, 3);
  //spi.frequency(1000000);
}

void SetMotorPower(int motor, int power)
{
  if (motor < 1 || motor > 2)
    return;

  if (power > 100) {
      power = 100;
  } else if (power < -100) {
      power = -100;
  }

  uBit.io.P16.setDigitalValue(0);
  if (motor == 1) {
      spi.write(kRM_Motor1Power);
  } else {
      spi.write(kRM_Motor2Power);
  }
  spi.write(power);
  uBit.io.P16.setDigitalValue(1);
}

void PlayNote(int solfegeNote, int duration) {
  uBit.io.P16.setDigitalValue(0);
  spi.write(kRM_NoteLength);
  spi.write(duration);
  spi.write(kRM_NoteSolfege);
  spi.write(solfegeNote);
  uBit.io.P16.setDigitalValue(1);
}

void PlayNoteStream(char streamOpCode) {
  // Put a note opcode in the stream.
  uBit.io.P16.setDigitalValue(0);
  spi.write(kRM_NoteStream);
  spi.write(streamOpCode);
  uBit.io.P16.setDigitalValue(1);
}

void PlayNoteStream(const char* streamOpCode, int length) {
  // Put a note in ote stream.
  uBit.io.P16.setDigitalValue(0);
  for ( int i = 0; i < length; i++ ) {
    spi.write(kRM_NoteStream);
    spi.write(streamOpCode[i]);
  }
  uBit.io.P16.setDigitalValue(1);
}

#if 0
void SetMotorPower(int motor, int power)
{
  if (motor < 1 || motor > 2)
    return;

  uBit.io.P16.setDigitalValue(0);
  if (motor == 1) {
    //spi.write(0x81);
  } else {
    //spi.write(0x82);
  }
  // Write percentage +/- 100
  //spi.write(((power / 2) & 0x0000007F));
  uBit.io.P16.setDigitalValue(1);
}
#endif

void ReadTBCSystemStatus() {
  //char send[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  uBit.io.P16.setDigitalValue(0);
#if 0
char reply[8];
  // spi.write(0x80 | SYS_Status);
  spi.write(SYS_Status);  // Read class
  reply[0] = spi.write(0); //HW
  reply[1] = spi.write(1); //FW Y
  reply[2] = spi.write(2); //FW M
  reply[3] = spi.write(3); //FW D
  reply[4] = spi.write(4); // Self test 1
  reply[5] = spi.write(5); // Self test 2
  reply[6] = spi.write(6); // TBD
  reply[7] = spi.write(7); // TBD
#endif
uBit.io.P16.setDigitalValue(1);
}

void ServoStop() {
  /*
  uBit.io.P16.setDigitalValue(0);
  spi.write(0x80 | SERVO_Run);
  spi.write(0x00);    // turn off all servos
  //fiber_sleep(100);
  uBit.io.P16.setDigitalValue(1);
  //fiber_sleep(100);
  */
}


void ServoMessage(int, int) {
  /*
  uBit.io.P16.setDigitalValue(0);
  spi.write(0x80 | SERVO_Run);
  spi.write(0x07);
  //fiber_sleep(100);

  if (servo == 1) {
    spi.write(0x80 | SERVO_Set1);
    spi.write((period & 0x7f00) >> 8);
    spi.write((period & 0xff));
    spi.write(0x00);
    spi.write(0x00);
    //fiber_sleep(100);
  } else if (servo == 2) {
    spi.write(0x80 | SERVO_Set2);
    spi.write((period & 0x7f00) >> 8);
    spi.write((period & 0x00ff));
    spi.write(0x00);
    spi.write(0x00);
    //fiber_sleep(100);
  }
//  spi.write(0x80 | SERVO_Run);
//  spi.write(0x07);
  uBit.io.P16.setDigitalValue(1);
  //fiber_sleep(100);
  */
}
#if 0
void PlayNoteAsciiStream(const char* streamOpCode, int length) {

    teak::tstring   ns(streamOpCode, streamOpCode+length);
    int newLength = -1;

    while (!ns.empty()) {
        newLength = 0;
        // FIRST should be note root, in caps
        char c = ns.get_char();
        if (!(c>= A && c <= G))
            break;
        int nidx = c - 'A'
        c = ns.get_char();

        // Next is optiona sharp notation
        if (c == 'S' || c == '#') {
            // Its a sharp bump up 1/2 step
            nidx++
            c = ns.get_char();
        } else if (c == 'b') {
            nidx--
            c = ns.get_char();
        }

        // Next is optiona octave
        if (c>= '0'  && c <= '8') {
            int octave = c - '0';
            nindx = nindx + (ocatve * 12)
            c = ns.get_char();
        }

        ksNoteL0 = 100,		// Sets last note but does play it.
        ksNoteL64th,
    	ksNoteL32nd,
    	ksNoteL16th,
    	ksNoteL8th,
    	ksNoteL4th,
    	ksNoteLHalf,
    	ksNoteLWhole,
    	ksNoteLTriplet,  	// Note length is 1/3 twice the current
    	ksNoteLDot,				// Note length is lengthened by 1/2
    	ksNoteLFermata,  	// Note length is doubled

        // Next is optional length
        if (c == ':') {
            c = ns.get_char();
            switch (c) {
                case 'x':  newLength = ksNoteL64th; break;
                case 't':  newLength = ksNoteL32nd; break;
                case 's':  newLength = ksNoteL16th; break;
                case 'e':  newLength = ksNoteL8th; break;
                case 'q':  newLength = ksNoteL4th; break;
                case 'h':  newLength = ksNoteLHalf; break;
                case 'w':  newLength = ksNoteLWhole; break;
                case 'd':  newLength = ksNoteLDot; break;
                case '3':  newLength = ksNoteLTriplet; break;
                case 'f':  newLength = ksNoteLFermata; break;
                case 'z':  newLength = ksNoteL0; break;
                default: newLength = -1; break;
            }
        }

        if (c == ',') {
            continue;
        }
    }
#if 0
  // Set up defaults
  int octave = 4;
  int period = ksNoteL4th;
 enum {tkNote, tkOctave, tkTime};
  int i = 0;
  char* c = streamOpCode[i];
  while (i < length) {
    switch(state) {
      case tkNote:
          if (c>= 'A' && c < = 'G') {
            note = ksNoteA0 + c - 'A';
            i++;
          } else if (c == 'R') {
            note = ksNoteRest;
            i++;
          }
          state = tkOctave;
          break;
      case tkOctave:
          if (c => '0' && c <= '9') {
            octave = c - '0';
            i++;
          }
          state = tkTimeSep;
      case tkTimeSep:
          if (c == ':') {
            state = tkTimeSep;
          }
      case tkTime:
          if (c == ':') {

          }
    }

    if (c>= 'A' && c < = 'G') {
      note = ksNoteA0 + c - 'A';
      i++;
    } else if (c == 'R') {
      note = ksNoteRest;
      i++;
    }

    if (i < length) {
      c = streamOpCode[i];
      if (c => '0' && c <= '9') {
        octave = c - '0';
      } else
    }
  }

  // Put a note in ote stream.
  uBit.io.P16.setDigitalValue(0);
  for ( int i = 0; i < length; i++ ) {
    spi.write(kRM_NoteStream);
    spi.write(streamOpCode[i]);
  }
  uBit.io.P16.setDigitalValue(1);
  #endif
}
#endif

void stopAll()
{
  SetMotorPower(1, 0);
  SetMotorPower(2, 0);
  uBit.display.clear();
}
