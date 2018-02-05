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
#include "MicroBitUARTServiceFixed.h"
#include "TeakTask.h"
#include "TrashbotsController.h"

MicroBit uBit;
MicroBitI2C i2c = MicroBitI2C(I2C_SDA0, I2C_SCL0);

// Could the code tap into the lower layer, and look for complete expression
// that would be better. It will be helpful to have sctatter string support.
ManagedString eom(";");
MicroBitUARTServiceFixed *uart;
SPI spi(MOSI, MISO, SCK);

void SetMotorPower(int motor, int power)
{
  if (motor < 1 || motor > 2)
    return;

  if (power > 100) {
    power = 100;
  } else if (power < -100) {
    power = -100;
  }

  uBit.io.P1.setDigitalValue(0); // for scope.
  uBit.io.P16.setDigitalValue(0);
  if (motor == 1) {
    spi.write(kRM_Motor1Power);
  } else {
    spi.write(kRM_Motor2Power);
  }
  if (power >= 0) {
    spi.write(power);
  } else {
    spi.write(power);
  }
  uBit.io.P16.setDigitalValue(1);
  uBit.io.P1.setDigitalValue(1);
}

void PlayNote(int solfegeNote) {
  uBit.io.P16.setDigitalValue(0);
  spi.write(kRM_NoteSolfege);
  spi.write(solfegeNote);
  uBit.io.P16.setDigitalValue(1);
}


void onConnected(MicroBitEvent event )
{
    // Need to note this in a different way
    uBit.display.print('C');
    gTaskManager.Event(event);
    return;
}

void onDisconnected(MicroBitEvent event)
{
   // Need to note this in a different way
    uBit.display.print('D');
    gTaskManager.Event(event);
    return;
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
  //teak::tstring command(s->data);
  char* str = s->data;
  int len = strlen(str);
  int value = 0;
  // one command to start with, the picture display
  // P0123456789:
  // the numebers are hex, the five bits are packed into two hex digits.
  if ((strncmp(str, "(px:", 4) == 0) && len >= 14) {
    str += 4;
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
  } else if ((strncmp(str, "(sr:", 4) == 0) && len >= 5) {
    str += 4;
    value = atoi(str);
    uBit.io.P1.setServoValue(value);
    uBit.display.scroll("S");
  } else if ((strncmp(str, "(m:", 3) == 0) && len >= 4) {
    str += 3;
    if(strncmp(str, "(1 2)", 5) == 0){
      if(strncmp(str + 6, "d", 1) == 0){
        value = atoi(str + 8);
        SetMotorPower(1, value);
        SetMotorPower(2, -value);
      }
    } else if(strncmp(str, "1", 1) == 0) {
      if(strncmp(str + 2, "d", 1) == 0) {
        value = atoi(str + 4);
        SetMotorPower(1, value);
      }
    } else if(strncmp(str, "2", 1) == 0) {
      if(strncmp(str + 2, "d", 1) == 0) {
        value = atoi(str + 4);
        SetMotorPower(2, -value);
      }
    }
} else if ((strncmp(str, "(nt:", 4) == 0) && len >= 5) {
    // Notes come in the form nnn where n is a the integer frequency
    value = atoi(str + 4);
    PlayNote(value);
  } else if ((strncmp(str, "(stop)", 6) == 0)) {
    stopAll();
  } else {
    uBit.display.scroll(str);
  }

  s->decr();
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    spi.format(8, 3);
     spi.frequency(1000000);

    //TBCInit();
    //uBit.display.print('A');

    // Set to be initially off.
    // setAdvertising(bAdvertising);

    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_EVT_ANY, &gTaskManager, &TeakTaskManager::Event);
    // uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    // End of packet detectd on incomming message
    uBit.messageBus.listen(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_DELIM_MATCH, onData);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, &gTaskManager, &TeakTaskManager::Event);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, &gTaskManager, &TeakTaskManager::Event);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_EVT_ANY, &gTaskManager, &TeakTaskManager::Event);
    uBit.messageBus.listen(MICROBIT_ID_DISPLAY, MICROBIT_EVT_ANY, &gTaskManager, &TeakTaskManager::Event);

    uart = new MicroBitUARTServiceFixed(*uBit.ble, 32, 32);
    uart->eventOn(eom, ASYNC);

    // Note GATT table size increased from default in MicroBitConfig.h
    // #define MICROBIT_SD_GATT_TABLE_SIZE             0x500

    // Run the main loop
    MicroBitEvent tick(MICROBIT_ID_TIMER, 0, CREATE_ONLY);
    int tickCount = 0;

    while(1) {
        tickCount++;
        fiber_sleep(100);
        tick.value = tickCount;
        gTaskManager.Event(tick);
      }

    // release_fiber();
}
