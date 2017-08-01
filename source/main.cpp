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

//#include "teak/source/tstring.h"
//#include "teak/source/tnode.h"

MicroBit uBit;
MicroBitI2C i2c = MicroBitI2C(I2C_SDA0, I2C_SCL0);
void ServoMessage(int period, int servo);
void ServoStop();
void stopAll();

MicroBitAccelerometer accelerometer = MicroBitAccelerometer(i2c);
MicroBitUARTServiceFixed *uart;

#include "TrashbotsController.h"

// Could the code tap into the lower layer, and look for complete expression
// that would be better. It will be helpful to have sctatter string support.
ManagedString eom(";");

SPI spi(MOSI, MISO, SCK);

class PWMServoMotor {
public:
  PWMServoMotor(int servo);
  static int g_powerMap[];
  int currentPower;
  void Enable(bool enabled);
  void SetPower(int power,int servo);
  void BumpPower();
  int m_enabled;
  int m_delta;
  int m_servo;
  int m_power;
};

//int PWMServoMotor::g_powerMap [] = { 60, 69, 75, 85, 98, 105, 115 };
int PWMServoMotor::g_powerMap [] = { 1500, 1200, 1100, 1500, 1200, 1100, 1500 };

void PWMServoMotor::Enable(bool enabled) {
  m_enabled = enabled;
  if (enabled) {
//    SetPower(m_power);
  } else {
//    SetPower(3);
  }
}

PWMServoMotor::PWMServoMotor(int servo) {
  m_servo = servo;
  m_delta = 1;
  m_power = 3;
  m_enabled = true;
//  SetPower(m_power);
}

void PWMServoMotor::BumpPower() {
  m_power += m_delta;
  if (m_power > 6) {
    m_power = 5;
    m_delta = -1;
  } else if (m_power < 0) {
    m_power = 1;
    m_delta = 1;
  }
  //SetPower(m_power);
}

int g_servoPWMArray[] = {
  1310,1370,1380,1390,1395,1400,1405,1410,1415,1420,
  1450,
  1490,1500,1505,1510,1515,1520,1525,1535,1555,1590
};

void PWMServoMotor::SetPower(int power, int servo) {
  // Convert +/- 100% value to 0-20 index
  int index = power/10;
  index += 10;
  if (index < 0) {
     index = 0;
  } else if (index > 20) {
    index = 20;
  }

  int uSecPeriod = g_servoPWMArray[index];
//  uBit.display.print(uSecPeriod);
  ServoMessage(uSecPeriod, servo);
  /*
  if (m_servo == 0) {
    uBit.io.P0.setServoValue(pwm);
  } else if (m_servo == 1) {
    uBit.io.P1.setServoValue(pwm);
  } else if (m_servo == 2) {
    uBit.io.P2.setServoValue(pwm);
  }
  */
}

int connected = 0;

PWMServoMotor s0(0);
PWMServoMotor s1(1);
PWMServoMotor s2(2);

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

void ServoStop() {
  uBit.io.P16.setDigitalValue(0);
  spi.write(0x80 | SERVO_Run);
  spi.write(0x00);    // turn off all servos
  fiber_sleep(100);
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
  fiber_sleep(100);

  if (servo == 1) {
    spi.write(0x80 | SERVO_Set1);
    spi.write((period & 0x7f00) >> 8);
    spi.write((period & 0xff));
    spi.write(0x00);
    spi.write(0x00);
    fiber_sleep(100);
  } else if (servo == 2) {
    spi.write(0x80 | SERVO_Set2);
    spi.write((period & 0x7f00) >> 8);
    spi.write((period & 0x00ff));
    spi.write(0x00);
    spi.write(0x00);
    fiber_sleep(100);
  }
//  spi.write(0x80 | SERVO_Run);
//  spi.write(0x07);
  uBit.io.P16.setDigitalValue(1);
  fiber_sleep(100);
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
        s0.SetPower(value,1);
        s1.SetPower(-value,2);
      }
    } else if(strncmp(str, "1", 1) == 0){
      if(strncmp(str + 2, "d", 1) == 0){
        value = atoi(str + 4);
        s0.SetPower(value,1);
      }
    } else if(strncmp(str, "2", 1) == 0){
      if(strncmp(str + 2, "d", 1) == 0){
        value = atoi(str + 4);
        s1.SetPower(-value,2);
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

void stopAll()
{
  s0.SetPower(0, 1);
  s1.SetPower(0, 2);

  uBit.display.clear();
}

int servoValueA = 75;
int servoValueB = 70;

void onButtonA(MicroBitEvent)
{
  //uBit.display.print('s');
  s0.SetPower(50,1);
//  uBit.display.print(s1.m_power);
/*
  uBit.io.P0.setServoValue(servoValueA);
  uBit.display.print(servoValueA-25/10);
  servoValueA += 10;
  if (servoValueA > 110) {
    servoValueA = 75;
  }
*/
  if (connected == 0) {
      return;
  }
  uart->send(ManagedString("(button-down(a))"));
  //  uBit.display.scroll("T");
}

void onButtonB(MicroBitEvent)
{
  PlayNote(3, 4);
  s0.SetPower(0,1);
  s0.SetPower(0,2);

//  ServoStop();

    if (connected == 0) {
        return;
    }
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
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_HOLD, onABEvent);
      uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, onABEvent);

    // Note GATT table size increased from default in MicroBitConfig.h
    // #define MICROBIT_SD_GATT_TABLE_SIZE             0x500
    uart = new MicroBitUARTServiceFixed(*uBit.ble, 32, 32);
    //uBit.display.scroll("");
    uart->eventOn(eom, ASYNC);

    accelerometer.setRange(4);
//    int p = accelerometer.getPeriod();
//    int r = accelerometer.getRange();
  /*  uBit.display.scroll(p);
    uBit.display.scroll("/");
    uBit.display.scroll(r);
*/
    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    int dout = 0;
  //  char c = 0;
    while(1) {
        fiber_sleep(100);

      //  uBit.io.P1.setServoValue(servoValue);
    //    uBit.io.P0.setDigitalValue(dout);
        dout = !dout;
      //  spi.write(c++);

        if (connected) {
//          uart->putc('z', ASYNC);
        }
      }

    //    release_fiber();
    #endif
}
