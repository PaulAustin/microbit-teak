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
short versionNumber;
bool connected;
int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    TBCInit();
    spi.format(8, 3);
    spi.frequency(1000000);
    gTaskManager.Setup();
	versionNumber = -10;
	connected = false;
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


    const char* accMessage = "(accel:%d)";
    const char* tempMessage = "(temp:%d)";

    while(1) {
		// uBit.serial.send("still here 1");
        tickCount++;
		// uBit.serial.send("still here 2");
        fiber_sleep(50);
		// uBit.serial.send("still here 3");
        tick.value = tickCount;
		// uBit.serial.send("still here 4");
        gTaskManager.MicrobitDalEvent(tick);
		// uBit.serial.send("still here 5");
        
        //processAccelerometerData(accelerometerData);
		if (connected)
		{
			int accelerometerData = uBit.accelerometer.getX();
			// uBit.serial.send("still here 6");
			snprintf(buffer, sizeof(buffer), accMessage, accelerometerData);
			uBit.serial.send(buffer);
			// uBit.serial.send("still here 7");
			uart->send((uint8_t *)buffer, strlen(buffer));
			// uBit.serial.send("still here 8");

			//processThermometerData(thermometerData);
			int thermometerData = thermometer.getTemperature();
			// uBit.serial.send("still here 9");
			snprintf(buffer, sizeof(buffer), tempMessage, thermometerData);
			uBit.serial.send(buffer);
			uart->send((uint8_t *)buffer, strlen(buffer)); //causes crash when app connects then disconnects
			// uBit.serial.send("still here 11");
			// uBit.serial.send(" ");
			uBit.serial.send(versionNumber);
			if (versionNumber > 0)
			{
				const char* versionMessage = "(vs:%d)";
				snprintf(buffer, sizeof(buffer), versionMessage, versionNumber);
				// uBit.serial.send(buffer);
				uart->send((uint8_t *)buffer, strlen(buffer));
			}
		}
		uBit.serial.send(tickCount);
		uBit.serial.send("\r\n");

    }
    // release_fiber();
}
