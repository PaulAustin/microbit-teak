
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

#include <MicroBit.h>
#include "TrashbotsController.h"

extern MicroBit uBit;
SPI spi(MOSI, MISO, SCK);

void SetMotoPower(int motor, int power)
{
  if (motor < 1 || motor > 2)
    return;

  uBit.io.P16.setDigitalValue(0);
  if (motor == 1) {
    spi.write(0x80 | MOTOR_Set1);
  } else {
    spi.write(0x80 | MOTOR_Set2);
  }
  spi.write((power & 0x0000FF00) >> 8);
  spi.write((power & 0x000000FF));
  spi.write(0x00); // Zero means run indefinitely
  spi.write(0x00); // Zero means run indefinitely
  uBit.io.P16.setDigitalValue(1);
}


void ReadTBCSystemStatus() {
  //char send[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  char reply[8];
  uBit.io.P16.setDigitalValue(0);
#if 1
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
#else
  spi.write (send, 8, reply, 8);
#endif

uBit.io.P16.setDigitalValue(1);
uBit.display.scroll((int)reply[4]);
//uBit.display.scroll(reply[2]);

/*
SpiMem_Write(SYS_Status, SYS_Status_HW_Rev_Num, 1); 0
SpiMem_Write(SYS_Status, SYS_Status_FW_Rev_Year, 17); 1
SpiMem_Write(SYS_Status, SYS_Status_FW_Rev_Month, 8); 2
SpiMem_Write(SYS_Status, SYS_Status_FW_Rev_Date, 42); 3
SpiMem_Write(SYS_Status, SYS_Status_FW_Self_Test1, 35); 4
SpiMem_Write(SYS_Status, SYS_Status_FW_Self_Test2, 36); 5
*/

}

void ServoStop() {
  uBit.io.P16.setDigitalValue(0);
  spi.write(0x80 | SERVO_Run);
  spi.write(0x00);    // turn off all servos
  //fiber_sleep(100);
  uBit.io.P16.setDigitalValue(1);
  //fiber_sleep(100);
}

/*void ServoOff(int period, int servo) {
  uBit.io.P16.setDigitalValue(0);
  spi.write(0x80 | SERVO_Run);
  spi.write(0x00);
  uBit.io.P16.setDigitalValue(1);
}
*/

void ServoMessage(int period, int servo) {
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
}

void PlayNote(int note, int octave) {
  // set chip enable
  if (note > 6 || note < 0) {
    note = 0;
  }
  if (octave > 8 || octave < 0) {
    octave = 4;
  }
  uBit.io.P16.setDigitalValue(0);
  spi.write(0x80 | BEEP_PlayNote);
  spi.write((note << 4) | octave);
  uBit.io.P16.setDigitalValue(1);
  //fiber_sleep(100);

/*
  // Set note
  fiber_sleep(50);
  spi.write(0x80 | BEEP_Note0);
  spi.write(frequency >> 8);
  spi.write(frequency & 0xFF);
  spi.write(duration >> 8);
  spi.write(duration & 0xFF);
  fiber_sleep(100);

  // Play note
  spi.write(0x80 | BEEP_Play);
  spi.write(0);
  spi.write(0);

  */
}

void stopAll()
{
  SetMotoPower(1, 0);
  fiber_sleep(100);
  SetMotoPower(2, 0);
  uBit.display.clear();
}
