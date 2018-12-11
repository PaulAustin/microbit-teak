/*
Copyright (c) 2018 Paul Austin.

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

extern MicroBit uBit;

//------------------------------------------------------------------------------
// Set up the intial task to be the boot task, this will
// run the startup screen

/*
Menu state machines.

1. On power up a short animation and initial display of m_name

2. Top level menu show small set of optipons
    A - Outputs ( motors)
    B - Sensors ( three sub option Accelerator, Gyro, Temp)
    C - User save programs

3. A Special mode for helping establish BT, also shows m_name

The Taskmanager manages all transistion by watching for globally detectable
events.
  A-LONG -> Go into/outof BT monitor mode.
  B-LONG -> Run (from top) or Reset to top
  A Scroll left in Task
  B Scroll right in Task
*/

// Images are unpacked or merged in one buffer before
// being transfered to the main display.
//ZZZextern TeakTask *gTasks[];
TeakTaskManager gTaskManager;

ManagedString eom(";");
MicroBitUARTServiceFixed *uart;

//------------------------------------------------------------------------------
TeakTask::TeakTask() {
    m_image = 0;
    m_asyncImage = false;
    m_leftTask = NULL;
    m_rightTask = NULL;
}

//------------------------------------------------------------------------------
void TeakTask::SetAdjacentTasks(TeakTask* leftTask, TeakTask* rightTask)
{
    m_leftTask = leftTask;
    m_rightTask = rightTask;
}

//------------------------------------------------------------------------------
TeakTaskManager::TeakTaskManager()
{
  m_currentTask = NULL;
}

//------------------------------------------------------------------------------
void TeakTaskManager::Setup()
{
    gpEmojiTask->SetAdjacentTasks(gpMotorTask, gpSensorTask);
    gpMotorTask->SetAdjacentTasks(gpBlueToothTask, gpEmojiTask);
    gpBlueToothTask->SetAdjacentTasks(gpSensorTask, gpMotorTask);
    gpSensorTask->SetAdjacentTasks(gpEmojiTask, gpBlueToothTask);

    // Start with the boot task.
    SwitchTo(gpBootTask);

    // Connect event listeners
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_EVT_ANY, this, &TeakTaskManager::MicrobitDalEvent);
    uBit.messageBus.listen(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_DELIM_MATCH, this, &TeakTaskManager::MicrobitBtEvent);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, this, &TeakTaskManager::MicrobitDalEvent);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, this, &TeakTaskManager::MicrobitDalEvent);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_EVT_ANY, this, &TeakTaskManager::MicrobitDalEvent);
    uBit.messageBus.listen(MICROBIT_ID_DISPLAY, MICROBIT_EVT_ANY, this, &TeakTaskManager::MicrobitDalEvent);

    // Get the micro:bit name and make it upper case for legibility on LEDs
    strncpy(m_name, microbit_friendly_name(), sizeof(m_name));
    char* upperName = m_name;
    for (int i = strlen(m_name); i ; i--) {
       *upperName = toupper(*upperName);
       upperName++;
    }

    // Note GATT table size increased from default in MicroBitConfig.h
    // #define MICROBIT_SD_GATT_TABLE_SIZE             0x500
    uart = new MicroBitUARTServiceFixed(*uBit.ble, 32, 32);
    uart->eventOn(eom, ASYNC);
}

//------------------------------------------------------------------------------
void TeakTaskManager::SwitchTo(TeakTask* task)
{
    m_currentTask = task;
}

//------------------------------------------------------------------------------
void TeakTaskManager::MicrobitDalEvent(MicroBitEvent event)
{
    if (event.source == MICROBIT_ID_BLE) {
        if (event.value == MICROBIT_BLE_EVT_CONNECTED) {
            m_btConnected = true;
            uBit.display.print('C');
        } else if (event.value == MICROBIT_BLE_EVT_DISCONNECTED) {
            m_btConnected = false;
            uBit.display.print('D');
        }
    } else if (event.value == MICROBIT_BUTTON_EVT_HOLD) {
        if (event.source == MICROBIT_ID_BUTTON_B) {
          // uBit.display.print('G');
        }
    } else if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
         if (event.source == MICROBIT_ID_BUTTON_A) {
             uart->send(ManagedString("(a)"));
         } else if (event.source == MICROBIT_ID_BUTTON_B) {
             uart->send(ManagedString("(b)"));
         } else if (event.source == MICROBIT_ID_BUTTON_AB) {
             uart->send(ManagedString("(ab)"));
         }
    }

    if (m_currentTask != NULL) {
        if (event.source == MICROBIT_ID_BUTTON_B && event.value == MICROBIT_BUTTON_EVT_HOLD) {
            // The Task swap key is treated as a special event
            event.source = MICROBIT_ID_TASK_SWAP;
            m_currentTask->Event(event);
            event.source = MICROBIT_ID_BUTTON_B;
          }

        m_currentTask->Event(event);
    }

    if (!m_currentTask->AsyncImage()) {
        int newImage = m_currentTask->PackedImage();
        if (newImage != m_currentImage) {
            // If task image has changed the push it to the LEDs
            PBmapUnpack(newImage, uBit.display.image.getBitmap(), uBit.display.image.getWidth());
            m_currentImage = newImage;
        }
    }
}

//------------------------------------------------------------------------------
int PBmapUnpack(int pbmap, uint8_t* bytes, int width)
{
    int bits = pbmap;
    uint8_t* rowBytes = bytes;
    for (int i=0; i < 5; i++) {
        rowBytes[0] = (bits & (0x01 << 4));
        rowBytes[1] = (bits & (0x01 << 3));
        rowBytes[2] = (bits & (0x01 << 2));
        rowBytes[3] = (bits & (0x01 << 1));
        rowBytes[4] = (bits & (0x01 << 0));
        rowBytes += width;
        bits = bits >> 5;
    }
    return 0;
}

#if 0
// Just for reference
#define MICROBIT_BUTTON_EVT_DOWN                1
#define MICROBIT_BUTTON_EVT_UP                  2
#define MICROBIT_BUTTON_EVT_CLICK               3
#define MICROBIT_BUTTON_EVT_LONG_CLICK          4
#define MICROBIT_BUTTON_EVT_HOLD                5
#define MICROBIT_BUTTON_EVT_DOUBLE_CLICK        6

#define MICROBIT_BUTTON_LONG_CLICK_TIME         1000
#define MICROBIT_BUTTON_HOLD_TIME               1500

#define MICROBIT_BUTTON_STATE                   1
#define MICROBIT_BUTTON_STATE_HOLD_TRIGGERED    2
#define MICROBIT_BUTTON_STATE_CLICK             4
#define MICROBIT_BUTTON_STATE_LONG_CLICK        8
#endif

//------------------------------------------------------------------------------
void setAdvertising(bool state)
{
    if (state) {
        uBit.bleManager.setTransmitPower(6);
        uBit.bleManager.ble->setAdvertisingInterval(200);
        uBit.bleManager.ble->gap().setAdvertisingTimeout(0);
        uBit.bleManager.ble->gap().startAdvertising();
    } else {
        uBit.bleManager.stopAdvertising();
    }
}

//------------------------------------------------------------------------------
// Could the code tap into the lower layer, and look for complete expression
// that would be better. It will be helpful to have sctatter string support.
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

void PlayNoteAsync(int solfegeNote, int duration) {
  // Put a note in ote stream.
  uBit.io.P16.setDigitalValue(0);
  spi.write(kRM_NoteStream);
  spi.write(solfegeNote);
  uBit.io.P16.setDigitalValue(1);
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
void TeakTaskManager::MicrobitBtEvent(MicroBitEvent)
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
        if(strncmp(str, "(1 2)", 5) == 0) {
            if(strncmp(str + 6, "d", 1) == 0) {
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
        // Notes come in the form nn where n is the
        // piano key number
        value = atoi(str + 4);
        if (value <= 13) {
          // early version pay in register0 ( notes 1-12 ( + next C))
          // bump to C4 (key number 40)
          value += 39;
        }
        PlayNote(value, 64);
    } else if ((strncmp(str, "(pr:", 4) == 0) && len >= 5) {
        value = atoi(str + 4);
        uBit.display.scroll(value);
    } else if ((strncmp(str, "(stop)", 6) == 0)) {
        stopAll();
    } else {
        uBit.display.scroll(str);
    }
    s->decr();
}
