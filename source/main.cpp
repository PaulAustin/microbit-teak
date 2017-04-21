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

#include "MicroBit.h"
#include "MicroBitUARTServiceFixed.h"

MicroBit uBit;
//MicroBitI2C i2c = MicroBitI2C(I2C_SDA0, I2C_SCL0);
//MicroBitAccelerometer accelerometer = MicroBitAccelerometer(i2c);
MicroBitUARTServiceFixed *uart;

ManagedString eom(":");

 SPI spi(MOSI, MISO, SCK);

int connected = 0;

void onConnected(MicroBitEvent)
{
    uBit.display.scroll("C");
    connected = 1;
    return;
    // uart->putc('z', ASYNC);
    // mobile app will send ASCII strings terminated with the colon character
    ManagedString eom(":");
}

void onDisconnected(MicroBitEvent)
{
    uBit.display.scroll("D");
    connected = 0;
}

int hexCharToInt(char c) {
  if ((c >= '0') && (c <= '9')) {
    return c - '0';
  } else if ((c >= 'a') && (c <= 'f')) {
    return c - 'a' + 10;
  }  else if ((c >= 'A') && (c <= 'F')) {
    return c - 'A' + 10;
  }
  return 0;
}

MicroBitImage image(5,5);
int j = 0;
int pixVal = 1;
// Called when a delimiter is found.
void onData(MicroBitEvent)
{
  // If the event was called there should be a message
  ManagedString buff = uart->readUntil(eom, ASYNC);
  StringData *s = buff.leakData();
  // Internal buffer is null terminated as well.
  char* str = s->data;
  int len = strlen(str);
  // one command to start with, the picture display
  // P0123456789:
  // the numebers are hex, the five bits are packed into two hex digits.
  if ((strncmp(str, "(px", 3) == 0) && len >= 14) {
    str += 3;
    for (int i = 0; i < 5; i++) {
      char c1 = hexCharToInt(*str);
      str++;
      char c2 = hexCharToInt(*str);
      str++;
      image.setPixelValue(0, i, c1 & 0x01);
      image.setPixelValue(1, i, c2 & 0x08);
      image.setPixelValue(2, i, c2 & 0x04);
      image.setPixelValue(3, i, c2 & 0x02);
      image.setPixelValue(4, i, c2 & 0x01);
    }
    uBit.display.print(image);
  } else if ((strncmp(str, "(sv", 3) == 0) && len >= 5) {
    str += 3;
    uBit.display.scroll(len);
  } else if ((strncmp(str, "(mo", 3) == 0) && len >= 5) {
    str += 3;
    uBit.display.scroll(len);
  } else {
    uBit.display.scroll(len);
  }

  s->decr();
}

int servoValue = 30;

void onButtonA(MicroBitEvent)
{
  uBit.io.P1.setServoValue(servoValue);

  servoValue += 10;
  if (servoValue > 160) {
    servoValue = 30;
  }

    if (connected == 0) {
        return;
    }
    uart->send(ManagedString("(button-down(a))"));
  //  uBit.display.scroll("T");
}

void onButtonB(MicroBitEvent)
{
  //  spi.write('B');
    uBit.display.scroll("B");
//    uBit.io.P0.setDigitalValue(1);
    if (connected == 0) {
        return;
    }
    uart->send(ManagedString("(button-down(b))"));
}

void onButtonAB(MicroBitEvent)
{
    if (connected == 0) {
        return;
    }
    uart->send(ManagedString("(button-down (a b))"));
  //  uBit.display.scroll("!");
}

int main()
{
  #if  1
    // Initialise the micro:bit runtime.
    uBit.init();
    uBit.io.P0.setDigitalValue(0);
    spi.format(8,3);
    spi.frequency(1000000);

    uBit.io.P1.setServoValue(30);

    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_DELIM_MATCH, onData);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, onButtonAB);

    // Note GATT table size increased from default in MicroBitConfig.h
    // #define MICROBIT_SD_GATT_TABLE_SIZE             0x500
    uart = new MicroBitUARTServiceFixed(*uBit.ble, 32, 32);
    uBit.display.scroll("SDG5");
    uart->eventOn(eom, ASYNC);

    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    int dout = 0;
  //  char c = 0;
    while(1) {
        fiber_sleep(100);

      //  uBit.io.P1.setServoValue(servoValue);
        uBit.io.P0.setDigitalValue(dout);
        dout = !dout;
      //  spi.write(c++);

        if (connected) {
//          uart->putc('z', ASYNC);
        }
      }

    //    release_fiber();
    #endif
}
