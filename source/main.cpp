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

char buffer [20];
short versionNumber = 10;
int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    TBCInit();
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


    const char* accMessage = "(ac:%d)";
    const char* tempMessage = "(tp:%d)";

    while(1) {
        tickCount++;
        fiber_sleep(50);
        tick.value = tickCount;
        gTaskManager.MicrobitDalEvent(tick);

        
        //processAccelerometerData(accelerometerData);
        int accelerometerData = uBit.accelerometer.getX();
        snprintf(buffer, sizeof(buffer), accMessage, accelerometerData);
        uart->send((uint8_t *)buffer, strlen(buffer));

        //processThermometerData(thermometerData);
        int thermometerData = thermometer.getTemperature();
        snprintf(buffer, sizeof(buffer), tempMessage, thermometerData);
        uart->send((uint8_t *)buffer, strlen(buffer));

		if (versionNumber != -1)
		{
			const char* versionMessage = "(vs:%d)";
			snprintf(buffer, sizeof(buffer), versionMessage, versionNumber);
			uBit.serial.send(buffer);
			uart->send((uint8_t *)buffer, strlen(buffer));
		}
    }
    // release_fiber();
}
