/*   FANCY
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

#ifndef TRASHBOTS_CONTROLLER_SERVICE_H
#define TRASHBOTS_CONTROLLER_SERVICE_H

enum RegMap_t
{
	kRM_SystemFMVers1 = 1,
	kRM_SystemFMVers2,
	kRM_SystemStatus,
	kRM_SystemIdle,
	kRM_SystemStop,		     // Stop all IO,turn off all GPIO
	kRM_System5VAutoSleep,   // Disable for encoders to work all the time.
	kRM_GroupTillTrigger,
	kRM_Trigger,

	kRM_Motor1Power 	= 10,
	kRM_Motor1Rpm 		= 11,
	kRM_Motor1Break 	= 12,
	kRM_Motor1Cpr 		= 13,
	kRM_Motor1Countdown = 14,
	kRM_Motor1Encoder   = 15,
	// P, I , D  ??

	kRM_Motor2Power 	= 20,
	kRM_Motor2Rpm 		= 21,
	kRM_Motor2Break 	= 22,
	kRM_Motor2Cpr 		= 23,
	kRM_Motor2Countdowm = 24,
	kRM_Motor2Encoder	= 25,
	// P, I , D  ??


	kRM_MotorRpmSampleRate = 33, // typically about 10Hz

	kRM_Servo1Active	= 40, // 0:off,   1:0-2ms, O:custom ???
	kRM_Servo2Active 	= 41,
	kRM_Servo3Active 	= 42,
	// Room for more ...
	kRM_Servo1Position  = 50,
	kRM_Servo2Position  = 51,
	kRM_Servo3Position  = 52,
	// Room for more ...


	kRM_NoteTempo 		= 60,	// default
	kRM_NoteLength 		= 61,
	kRM_NoteSolfege 	= 62,	// piano key 1-88
	kRM_NoteHertz 		= 63,	// 0-14k??

	kRM_Gpio 			= 70,	    // BIT FIELD
	kRM_Gpio1 			= 71,	    // individual bits
	kRM_Gpio2 			= 72,	    // individual bits
	kRM_Gpio3 			= 73,	    // individual bits
	kRM_Gpio4 			= 74,	    // individual bits
	kRM_GpioOutputMask,	// BIT FIELD

	kRM_Count 			= 80,
};

extern SPI spi;

void TBCInit();
void PlayNote(int value, int duration);
void SetMotorPower(int motor, int power);
void stopAll();

#endif
