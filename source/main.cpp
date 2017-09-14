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

//#include "teak/source/tstring.h"
//#include "teak/source/tnode.h"

MicroBit uBit;
MicroBitI2C i2c = MicroBitI2C(I2C_SDA0, I2C_SCL0);

// Could the code tap into the lower layer, and look for complete expression
// that would be better. It will be helpful to have sctatter string support.
ManagedString eom(";");

MicroBitThermometer thermometer;
MicroBitAccelerometer accelerometer = MicroBitAccelerometer(i2c);
MicroBitUARTServiceFixed *uart;

int connected = 0;

void onConnected(MicroBitEvent)
{
    uBit.display.scroll("C");
    connected = 1;
    return;
    // uart->putc('z', ASYNC);
    // mobile app will send ASCII strings terminated with the colon character
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
        SetMotoPower(1, value);
        SetMotoPower(2, -value);
      }
    } else if(strncmp(str, "1", 1) == 0) {
      if(strncmp(str + 2, "d", 1) == 0) {
        value = atoi(str + 4);
        SetMotoPower(1, value);
      }
    } else if(strncmp(str, "2", 1) == 0) {
      if(strncmp(str + 2, "d", 1) == 0) {
        value = atoi(str + 4);
        SetMotoPower(2, -value);
      }
    }

} else if ((strncmp(str, "(nt:", 4) == 0) && len >= 5) {
    // Notes come in the form 'C4' note, octave
    str += 4;
    value = str[0] - 'A';
    if ((value < 0) || (value > 6)) {
      value =0;
    }
    int octave = str[1] - '0';
    PlayNote(value, octave);
    //uBit.display.print(value);
  } else if ((strncmp(str, "(stop)", 6) == 0)) {
    stopAll();
  } else {
    uBit.display.scroll(str);
  }

  s->decr();
}

int servoValueA = 75;
int servoValueB = 70;

void onButtonA(MicroBitEvent)
{
  uBit.display.print('a');
  SetMotoPower(1, 80);
  fiber_sleep(10);

  //ReadTBCSystemStatus();
  //uBit.display.print('s');
  //s0.SetPower(50,1);

  if (connected == 0) {
      return;
  }
  uart->send(ManagedString("(button-down(a))"));
}

void onButtonB(MicroBitEvent)
{
  uBit.display.print('b');
  stopAll();

  if (connected == 0) {
      return;
  }
  char str[80];
  sprintf(str, "(accel(%i))", accelerometer.getX()); //
  uart->send(ManagedString(str));

  char str2[80];
  sprintf(str2, "(temp(%i))", thermometer.getTemperature()); //
  uart->send(ManagedString(str2));
  //  uart->send(ManagedString("(button-down(b))"));
}

bool bAdvertising = false;
void setAdvertising(bool state);

MicroBitImage beaconOff("0,0,0,0,0\n0,0,0,0,0\n0,0,255,0,0\n0,0,0,0,0\n0,0,0,0,0\n");
MicroBitImage beaconOn("0,255,255,255, 0\n255,0,0,0,255\n255,0,255,0,255\n255,0,0,0,255\n0,255,255,255,0\n");

void setAdvertising(bool state)
{
  if (state) {
    uBit.display.print(beaconOn);
    uBit.bleManager.setTransmitPower(6);
    uBit.bleManager.ble->setAdvertisingInterval(200);
    uBit.bleManager.ble->gap().setAdvertisingTimeout(0);
    uBit.bleManager.ble->gap().startAdvertising();
  } else {
    uBit.display.print(beaconOff);
    uBit.bleManager.stopAdvertising();
  }
}

void onABEvent(MicroBitEvent evt)
{
  if (evt.value == MICROBIT_BUTTON_EVT_HOLD) {
    bAdvertising = !bAdvertising;
    setAdvertising(bAdvertising);
  }
}


int main()
{
  #if  1
    // Initialise the micro:bit runtime.
    uBit.init();

    // Set to be initially off.
    setAdvertising(bAdvertising);

    //uBit.io.P0.setDigitalValue(0);
    //uBit.io.P1.setDigitalValue(0);
    spi.format(8, 3);
    spi.frequency(1000000);

    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_DELIM_MATCH, onData);

#if 0
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, &gTaskManager, &TeakTaskManager::Event);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, &gTaskManager, &TeakTaskManager::Event);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_EVT_ANY, &gTaskManager, &TeakTaskManager::Event);
#else
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_HOLD, onABEvent);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, onABEvent);
#endif

    uart = new MicroBitUARTServiceFixed(*uBit.ble, 32, 32);
    uart->eventOn(eom, ASYNC);

    // Note GATT table size increased from default in MicroBitConfig.h
    // #define MICROBIT_SD_GATT_TABLE_SIZE             0x500

    accelerometer.setRange(4);
    //thermometer.set
//    int p = accelerometer.getPeriod();
//    int r = accelerometer.getRange();
  /*  uBit.display.scroll(p);
    uBit.display.scroll("/");
    uBit.display.scroll(r);
*/
    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
  //  char c = 0;

    // Run the main loop
    MicroBitEvent tick(MICROBIT_ID_TIMER, 0, CREATE_ONLY);

    while(1) {
        fiber_sleep(100);
        gTaskManager.Event(tick);
      }

    //    release_fiber();
    #endif
}
