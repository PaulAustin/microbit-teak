/*
Copyright (c) 2019 Trashbots.

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

#include <string>
#include <stdio.h>
#include <MicroBit.h>
#include "MicroBitUARTServiceFixed.h"
#include "TeakTask.h"
#include "TBCDriver.h"

MicroBit uBit;
MicroBitI2C i2c = MicroBitI2C(I2C_SDA0, I2C_SCL0);

MicroBitStorage storage;
MicroBitThermometer thermometer(storage);

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    spi.format(8, 3);
    spi.frequency(1000000);

    gTaskManager.Setup();

    // Run the main loop
    MicroBitEvent tick(MICROBIT_ID_TIMER, 0, CREATE_ONLY);
    int tickCount = 0;

    PlayNoteStream(ksNoteL8th);
    PlayNoteStream(ksNoteC4);
    PlayNoteStream(ksNoteD4);
    PlayNoteStream(ksNoteE4);
    PlayNoteStream(ksNoteF4);
    PlayNoteStream(ksNoteG4);
    PlayNoteStream(ksNoteA5);
    PlayNoteStream(ksNoteB5);
    PlayNoteStream(ksNoteC5);

    while(1) {
        tickCount++;
        fiber_sleep(50);
        tick.value = tickCount;
        gTaskManager.MicrobitDalEvent(tick);

        int accelerometerData = uBit.accelerometer.getX();
        //processAccelerometerData(accelerometerData);

        int thermometerData = thermometer.getTemperature();
        //processThermometerData(thermometerData);

        char accelString [30];
        snprintf(accelString, sizeof(accelString), "(accel:%d)", accelerometerData);
        char tempString [30];
        snprintf(tempString, sizeof(tempString), "(temp:%d)", thermometerData);
        uart->send(ManagedString(accelString));
        uart->send(ManagedString(tempString));
    }
    // release_fiber();
}
