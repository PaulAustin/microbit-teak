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

enum
{
	kRM_Reserved0 = 101,
	// READ
	kRM_Status,
	kRM_RpmSampleRate,
	kRM_Rpm1,
	kRM_Rpm2,
	// WRITE
	kRM_GoIdle,
	kRM_GPIO,
	kRM_Motor1Power,
	kRM_Motor2Power,
	kRM_Servo1,
	kRM_Servo2,
	kRM_Servo3,
	kRM_NoteTempo,		// default
	kRM_NotePitch,		// piano key 1-88
	kRM_NoteLength,
	kRM_Count,
};



enum  // FANCY
{
Reserved0,
SYS_Status,
SYS_Settings,
SYS_Extra01,
BEEP_PlayNote,
BEEP_PlayTune,
BEEP_Status,
BEEP_Tune0,
BEEP_Tune1,
BEEP_Tune2,
BEEP_Tune3,
BEEP_Settings,
BEEP_Extra20,
BEEP_Extra19,
BEEP_Extra18,
BEEP_Extra17,
BEEP_Extra16,
BEEP_Extra15,
BEEP_Extra14,
BEEP_Extra13,
BEEP_Extra12,
BEEP_Extra11,
BEEP_Extra10,
BEEP_Extra09,
BEEP_Extra08,
BEEP_Extra07,
BEEP_Extra06,
BEEP_Extra05,
BEEP_Extra04,
BEEP_Extra03,
BEEP_Extra02,
BEEP_Extra01,
SERVO_Run,
SERVO_Status,
SERVO_Period,
SERVO_Set1,
SERVO_Set2,
SERVO_Set3,
SERVO_Extra10,
SERVO_Extra09,
SERVO_Extra08,
SERVO_Extra07,
SERVO_Extra06,
SERVO_Extra05,
SERVO_Extra04,
SERVO_Extra03,
SERVO_Extra02,
SERVO_Extra01,
MOTOR_Run,
MOTOR_Status,
MOTOR_Set1,
MOTOR_Set2,
MOTOR_Encoder,
MOTOR_Extra03,
MOTOR_Extra02,
MOTOR_Extra01,
ENC_Count,
ENC_Speed,
ENC_Setttings,
ENC_Extra05,
ENC_Extra04,
ENC_Extra03,
ENC_Extra02,
ENC_Extra01,
GPIO_ReadPins,
GPIO_WritePins,
GPIO_Mode,
GPIO_Pullup,
GPIO_Extra04,
GPIO_Extra03,
GPIO_Extra02,
GPIO_Extra01,
regMAX
};

extern SPI spi;

void TBCInit();
void PlayNote(int value, int octave);
void SetMotorPower(int motor, int power);
void stopAll();

#endif
