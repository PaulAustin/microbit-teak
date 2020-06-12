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

#ifndef TRASHBOTS_CONTROLLER_SERVICE_H
#define TRASHBOTS_CONTROLLER_SERVICE_H

enum SolfegeNotes{
	ksNoteRest= 0,
	ksNoteA0 	= 1,	 	// Octave 0
	ksNoteAS0 = 2,
	ksNoteB0	= 3,
	ksNoteC0 	= 4,
	ksNoteCS0 = 5,
	ksNoteD0 	= 6,
	ksNoteDS0 = 7,
	ksNoteE0 	= 8,
	ksNoteF0 	= 9,
	ksNoteFS0 = 10,
	ksNoteG0 	= 11,
	ksNoteGS0 = 12,
	ksNoteA1 	= 13,		// Octave 1
	ksNoteAS1 = 14,
	ksNoteB1	= 15,
	ksNoteC1 	= 16,
	ksNoteCS1 = 17,
	ksNoteD1 	= 18,
	ksNoteDS1 = 19,
	ksNoteE1 	= 20,
	ksNoteF1 	= 21,
	ksNoteFS1 = 22,
	ksNoteG1 	= 23,
	ksNoteGS1 = 24,
	ksNoteA2 	= 25,		// Octave 2
	ksNoteAS2 = 26,
	ksNoteB2	= 27,
	ksNoteC2 	= 28,
	ksNoteCS2 = 29,
	ksNoteD2 	= 30,
	ksNoteDS2 = 31,
	ksNoteE2 	= 32,
	ksNoteF2 	= 33,
	ksNoteFS2 = 34,
	ksNoteG2 	= 35,
	ksNoteGS2 = 36,
	ksNoteA3 	= 25,		// Octave 1
	ksNoteAS3 = 26,
	ksNoteB3 	= 27,
	ksNoteC3 	= 28,
	ksNoteCS3 = 29,
	ksNoteD3 	= 30,
	ksNoteDS3 = 31,
	ksNoteE3 	= 32,
	ksNoteF3 	= 33,
	ksNoteFS3 = 34,
	ksNoteG3 	= 35,
	ksNoteGS3 = 36,
	ksNoteA4 	= 37,		// Octave 4
	ksNoteAS4 = 38,
	ksNoteB4 	= 39,
	ksNoteC4 	= 40,
	ksNoteCS4 = 41,
	ksNoteD4 	= 42,
	ksNoteDS4 = 43,
	ksNoteE4 	= 44,
	ksNoteF4 	= 45,
	ksNoteFS4 = 46,
	ksNoteG4 	= 47,
	ksNoteGS4 = 48,
	ksNoteA5 	= 49,
	ksNoteAS5 = 50,
	ksNoteB5 	= 51,
	ksNoteC5 	= 52,		// Octave 5
	ksNoteCS5 = 53,
	ksNoteD5 	= 54,
	ksNoteDS5 = 55,
	ksNoteE5 	= 56,
	ksNoteF5 	= 57,
	ksNoteFS5 = 58,
	ksNoteG5 	= 59,
	ksNoteGS5 = 60,
	ksNoteA6  = 61,		// Octave 6
	ksNoteAS6 = 62,
	ksNoteB6 	= 63,
	ksNoteC6 	= 64,
	ksNoteCS6 = 65,
	ksNoteD6 	= 66,
	ksNoteDS6 = 67,
	ksNoteE6 	= 68,
	ksNoteF6 	= 69,
	ksNoteFS6 = 70,
	ksNoteG6 	= 71,
	ksNoteGS6 = 72,
	ksNoteA7 	= 73,		// Octave 7
	ksNoteAS7 = 74,
	ksNoteB7 	= 75,
	ksNoteC7 	= 76,
	ksNoteCS7 = 77,
	ksNoteD7 	= 78,
	ksNoteDS7 = 79,
	ksNoteE7 	= 80,
	ksNoteF7  = 81,
	ksNoteFS7 = 82,
	ksNoteG7 	= 83,
	ksNoteGS7 = 84,
	ksNoteA8 	= 85,		// Octave 8`
	ksNoteAS8 = 86,
	ksNoteB8 	= 87,
	ksNoteC8 	= 88,

	// Additional codes for a note stream
	ksNoteL0 = 100,		// Sets last note but does play it.
	ksNoteL32nd,
	ksNoteL16th,
	ksNoteL8th,
	ksNoteL4th,
	ksNoteLHalf,
	ksNoteLWhole,
	ksNoteLTriplet,  	// Note length is 1/3 twice the current
	ksNoteLDot,				// Note length is lengthened by 1/2
	ksNoteLFermata,  	// Note length is doubled

	// Envelope modifiers
	ksNoteEStacatto = 110,
	ksNoteELagatto,
	ksNoteEDiatonicTrillo,
	ksNoteEGlissando,	// Pitch Blend from last note.

	ksNoteSetKey,			// All notes will be relative to the new key.
	ksNoteSetTempo,		// In BPM
};

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
	kRM_NoteStream 		= 66,	// 0-14k??

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
void PlayNoteStream(const char* value, int length);
void PlayNoteStream(char value);
void PlayNote(int value, int duration);
void SetMotorPower(int motor, int power);
void stopAll();
int ReadEncoder1();
int ReadEncoder2();
#endif
