
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
//SPI spi(MOSI, MISO, SCK);

void TBCInit()
{
  //spi.format(8, 3);
  //spi.frequency(1000000);
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


void ServoMessage(int period, int servo) {
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

void stopAll()
{
  SetMotorPower(1, 0);
  SetMotorPower(2, 0);
  uBit.display.clear();
}
