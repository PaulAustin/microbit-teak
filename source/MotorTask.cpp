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

#include <MicroBit.h>
#include "TeakTask.h"
#include "TBCDriver.h"
#include <string>

extern MicroBit uBit;

//------------------------------------------------------------------------------
// MotorTask - A task for direct control of the motors


const int FIRST_VALUE = 0xDEADBEEF;
const int THRESHOLD = 15;
const int TEST_POWER = 100;

#define just_started(encod) (encod == FIRST_VALUE)

class MotorTask : public TeakTask {
public:
    MotorTask();
    void Event(MicroBitEvent event);
    int counter = 0;
    int power1 = 0;
    int power2 = 0;
    int prevEncod1 = FIRST_VALUE;
    int prevEncod2 = FIRST_VALUE;
    bool fixed = false;
    int prevChange1 = 0;
    int prevChange2 = 0;
    int prevCorrection = 0;
    int revolutions = 0;
    int correction_storage = 0;
    int calibrate = 100;
    int one_values[THRESHOLD];
    int two_values[THRESHOLD];
private:
    int m_runningBits;
};
MotorTask gMotorTask;
TeakTask* gpMotorTask = &gMotorTask;

const int kMotoBase = PBMAP(
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(1, 0, 1, 0, 1),
    PBMAP_ROW(0, 1, 0, 1, 0),
    PBMAP_ROW(0, 0, 0, 0, 0),
    PBMAP_FRAME_COUNT(1));

const int kMotorLeftForward  =  PBMAP(PBMAP_ROW(0, 1, 0, 0, 0), 0, 0, 0, 0, 0);
const int kMotorRightForward =  PBMAP(PBMAP_ROW(0, 0, 0, 1, 0), 0, 0, 0, 0, 0);
const int kMotorLeftBack     =  PBMAP(0, 0, 0, 0, PBMAP_ROW(0, 1, 0, 0, 0), 0);
const int kMotorRightBack    =  PBMAP(0, 0, 0, 0, PBMAP_ROW(0, 0, 0, 1, 0), 0);

MotorTask::MotorTask()
{
    m_note = ksNoteC4;
    m_image = kMotoBase;
    m_runningBits = 0;
    //calibrate();
}

bool m1State = false;
bool m2State = false;
int skip2 = 0;
int skip1 = 0;





void MotorTask::Event(MicroBitEvent event)
{
    if (event.value == MICROBIT_BUTTON_EVT_CLICK) {
        m_image = kMotoBase;
        if (event.source == MICROBIT_ID_BUTTON_A) {
            if (calibrate == 100)
            {
                SetMotorPower(1, m1State ? 0 : -100);
                power1 = m1State ? 0 : -100;
                m1State = !m1State;
                if (m2State && m1State)
                    fixed = false;
            }
        } else if (event.source == MICROBIT_ID_BUTTON_B) {
            //SetMotorPower(1, m1State ? 0 : -80);
            //SetMotorPower(2, m2State ? 0 : 80);
            //m2State = !m2State;
            //m1State = !m1State;
            if (calibrate == 100)
            {
                SetMotorPower(2, m2State ? 0 : 100 + (correction_storage));
                power2 = m2State ? 0 : 100;
                m2State = !m2State;
                if (m2State && m1State)
                    fixed = false;
            }
            
        } else if (event.source == MICROBIT_ID_BUTTON_AB) {

            if (calibrate == 100)
            {
                int input_power = TEST_POWER;
                int correction = correction_storage * 1.0 / TEST_POWER * input_power;
                int corrected_power = (TEST_POWER+correction);
                SetMotorPower(1, m1State ? 0 : -TEST_POWER);
                SetMotorPower(2, m2State ? 0 : corrected_power);
                power1 = m1State ? 0 : -TEST_POWER;
                power2 = m2State ? 0 : corrected_power;
                // uBit.display.scroll(power2);
                m2State = !m2State;
                m1State = !m1State;
                if (m2State && m1State)
                    fixed = false;
            }
            
        }
        m_runningBits = 0;
        if (m1State) {
          m_runningBits |= kMotorLeftForward;
        }
        if (m2State) {
          m_runningBits |= kMotorRightForward;
        }
        //m_image |= m_runningBits;
    } else if(event.source == MICROBIT_ID_BUTTON_B && event.value == MICROBIT_BUTTON_EVT_HOLD) {
        // Shut down an pop to top
        m_runningBits = 0;
        SetMotorPower(1, 0);
        SetMotorPower(2, 0);
        PlayNoteStream(ksNoteG3);
        PlayNoteStream(ksNoteE3);
        PlayNoteStream(ksNoteC3);
        gTaskManager.SwitchTo(gpEmojiTask);
    } else if(event.source == MICROBIT_ID_BUTTON_B && event.value == MICROBIT_BUTTON_EVT_HOLD) {
        // Shut down an pop to top
        m_runningBits = 0;
        SetMotorPower(1, 0);
        SetMotorPower(2, 0);
        PlayNoteStream(ksNoteG3);
        PlayNoteStream(ksNoteE3);
        PlayNoteStream(ksNoteC3);
        gTaskManager.SwitchTo(gpEmojiTask);
    } else if(event.source == MICROBIT_ID_BUTTON_A && event.value == MICROBIT_BUTTON_EVT_HOLD) {
        gTaskManager.m_animating = true;
        uBit.display.scrollAsync(gTaskManager.BotName(), 70);
        PlayNoteStream(ksNoteC4);
        PlayNoteStream(ksNoteD4);
        PlayNoteStream(ksNoteE4);
        PlayNoteStream(ksNoteF4);
        PlayNoteStream(ksNoteG4);
        PlayNoteStream(ksNoteA5);
        PlayNoteStream(ksNoteB5);
        PlayNoteStream(ksNoteC5);
    } else if (event.source == MICROBIT_ID_TIMER) {
        if (calibrate == 100)
        {
            if (revolutions == 0)
            {
                SetMotorPower(2, TEST_POWER);
                //fiber_sleep(10);
                SetMotorPower(1, -TEST_POWER);
                power1 = -TEST_POWER;
                power2 = TEST_POWER;
                //fiber_sleep(200); 
            }
            if (revolutions < THRESHOLD)
            {
                int current1 = ReadEncoder1();
                int current2 = -1*ReadEncoder2();
                int change1 = prevEncod1 == FIRST_VALUE ? 0 : current1-prevEncod1;
                int change2 = prevEncod2 == FIRST_VALUE ? 0 : current2-prevEncod2;
                one_values[revolutions] = change1;
                two_values[revolutions] = change2;
            //uBit.display.scroll('S');
            //uBit.display.scroll(change1);
            //uBit.display.scroll(change2);
                prevEncod1 = current1;
                prevEncod2 = current2;
                revolutions++;
                fiber_sleep(50);
            }
            if (revolutions == THRESHOLD)
            {
                int sum1 = 0;
                int sum2 = 0;
                
                sort(one_values,one_values+THRESHOLD);
                sort(two_values,two_values+THRESHOLD);

                for (int i = 0; i < 5; i++)
                {
                    //uBit.display.scroll(one_values[i]);
                    //uBit.display.scroll(two_values[i]);
                    sum1 += one_values[i];
                    sum2 += two_values[i];
                }
                float average = (sum1+sum2);
                int temp = 1.0 * (sum2-sum1) / sum2 * power2;
                int median1 = one_values[THRESHOLD/2];
                int median2 = two_values[THRESHOLD/2];
                int median_average = 1.0 * (median1 + median2) / 2;
                int median_temp = 1.0 * (median2-median1) / median_average * power2;

                // uBit.display.scroll('S');
                // uBit.display.scroll(sum1);
                // uBit.display.scroll(sum2);
                uBit.display.scroll(median1);
                uBit.display.scroll(median2);
                uBit.display.scroll(median_temp);
                correction_storage = -median_temp;
                SetMotorPower(1, 0);
                SetMotorPower(2, 0);
                revolutions = 100;
            }
            //fiber_sleep(50);

        }
        if (calibrate != 100)
        {
            //uBit.display.scroll(calibrate);
            if (calibrate%5 == 0)
            {
                SetMotorPower(1, -100); //Set motor power to calibrate + 5
                fiber_sleep(10);
                SetMotorPower(2, 100);
                power1 = -100;
                power2 = 100;
                fiber_sleep(250);

            }
            if (calibrate%5 > 0)
            {
                if (calibrate%5 < 3)
                {
                    fiber_sleep(250);
                }
                if (calibrate%5 == 3)
                {
                    prevEncod1 = ReadEncoder1();
                    prevEncod2 = -1*ReadEncoder2();
                    fiber_sleep(2000);
                }

                if (calibrate%5 == 4)
                {
                    calibrate--;
                    int currEncod1 = ReadEncoder1();
                    currEncod1 = currEncod1;
                    if (prevEncod1 == FIRST_VALUE)
                    {
                        prevEncod1 = currEncod1;
                    }
                    int change1  = currEncod1 - prevEncod1;
                    
                    int currEncod2 = ReadEncoder2();
                    currEncod2 = (-1)*(currEncod2);
                    if (prevEncod2 == FIRST_VALUE)
                    {
                        prevEncod2 = currEncod2;
                    }
                    int change2  = currEncod2 - prevEncod2;

                    double correction_decimal = -1 * (change2 - change1);
                    correction_decimal = correction_decimal / (change2) ;
                    int correction = correction_decimal* power2;
                    //uBit.display.scroll(change1);
                    //uBit.display.scroll(change2);
                   
                    if (abs(correction) < 50)
                    {
                        calibrate++;
                        correction_storage = correction; //correction storage for calibrate + 1
                        SetMotorPower(1,0);
                        SetMotorPower(2,0);
                        power1 = 0;
                        power2 = 0;
                    }
                    else
                    {
                        fiber_sleep(2000);
                    }     
                }
            }
            calibrate++;
        }
        
        /*
        uBit.io.P16.setDigitalValue(0);
        //unsigned char send[5] = (int) {0, 1, 2, 3, 4};
        //unsigned char get[5] = {0, 0, 0, 0, 0};
        //get = spi.write(send);
        //int reply = (int)get[0];
        fiber_sleep(100);
        unsigned char send = 1;
        unsigned char reply = spi.write(send);
        uBit.display.scrollAsync(reply);
        uBit.io.P16.setDigitalValue(1);
        */
        //Call  CreateProcess() here to run test.cpp
        //m_image = reply;
        /*Read motor value here*/
        //m_image = -1;

        //uBit.io.P16.setDigitalValue(0);
        //reply = spi.write(-15);
        //uBit.io.P16.setDigitalValue(1);
        
        //uBit.display.scrollAsync("Let's go?");
        
        //If curr encod1 or prevencod1 is opposite of power entry, skip and don't check anythin
        /*
        if (skip1)
        {
            prevEncod1 = currEncod1;
            skip1 = false;
            return;
        }*/
        //skip two if just started, skip three later if negativel
        /*
        int currEncod1 = ReadEncoder1();
        int change1  = currEncod1 - prevEncod1;
        int actual_p1 = change1 / 100;
        if (skip1 > 0)
        {
            //PlayNoteStream(60);
            skip1--;
            prevEncod1 = currEncod1;
            prevChange1 = change1;
            return;
        }
        else if (just_started(prevEncod1))
        {
            prevEncod1 = currEncod1;
            skip1 = 2;
            prevChange1 = change1;
            return;
        }
        else if (currEncod1 < prevEncod1)
        {
            //PlayNoteStream(1);
            //uBit.display.scroll("negative");
            prevEncod1 = currEncod1;
            //uBit.display.scroll(change);
            skip1 = 3;
            prevChange1 = change1;
            return;
            //skip = true;
            //range is actually pretty high
        }//Skip integer, set to three
        else if (change1 > 500*abs(power1))
        {
            prevEncod1 = currEncod1;
            skip1 = 1;
            prevChange1 = change1;
            return;
        }
        else
        {            
            prevEncod1 = currEncod1;
            if (change1 < 1000 || prevChange1 < 1000)
            {
                change1 = 3333;
            }
            actual_p1 = change1 / 100;
            //uBit.display.scroll(change1);
            prevChange1 = change1;

            //uBit.display.scroll(actual_p1);
        }
        //int actual_p1 = (int) (change1 / 100);
        //uBit.display.scroll(actual_p1);
        


        
        int currEncod2 = ReadEncoder2();
        currEncod2 = (-1)*(currEncod2);
        int change2  = currEncod2 - prevEncod2;
        int actual_p2 = change2 / 100;
        //uBit.display.scroll(skip2);

        if (skip2 > 0)
        {   
            skip2--;
            prevEncod2 = currEncod2;
            prevChange2 = change2;
        }
        else if (just_started(prevEncod2))
        {
            prevEncod2 = currEncod2;
            skip2 = 2;
            prevChange2 = change2;
        }
        else if (currEncod2 < prevEncod2)
        {
            //uBit.display.scroll("negative");
            prevEncod2 = currEncod2;
            //uBit.display.scroll(change);
            skip2 = 3;
            prevChange2 = change2;
            //skip = true;
            //range is actually pretty high
        }//Skip integer, set to three
        else if (change2 > 500*abs(power2))
        {
            prevEncod2 = currEncod2;
            skip2 = 3;
            prevChange2 = change2;
        }
        else
        {            
            prevEncod2 = currEncod2;
            actual_p2 = change2 / 100;
            uBit.display.scroll(change2);
            //if (abs(power2 - actual_p2) < 10)
            //{
            //    return;
            //}
            if (!(change2 < 1000 || prevChange2 < 1000 || change2 < 10000))
            {
                int power_change = power2 - actual_p2; //positive - increase entry, negative - decrease entry
                if (power_change != 0)
                {
                    if (power2 == 100 && power_change > 0)
                      SetMotorPower(1, power1+power_change);
                    else
                      SetMotorPower(2, power2+power_change);
                }
                prevChange2 = change2;
            }
            
        }
        */
        /*
        int currEncod1 = ReadEncoder1();
        currEncod1 = currEncod1;
        if (prevEncod1 == FIRST_VALUE)
        {
            prevEncod1 = currEncod1;
        }
        int change1  = currEncod1 - prevEncod1;
        
        if (!(change1 < 7000 || change1 > 17000))
        {
            int actual_p1 = change1 / 100;
            int power_change = abs(power1) - actual_p1; //positive - incease entry, negative - decrease entry
            if (power_change != 0)
            {
              if (power1 == -100 && power_change > 0)
                SetMotorPower(2, power2-power_change);
              else
                SetMotorPower(1, power1-power_change);
            }
        }
        



        int currEncod2 = ReadEncoder2();
        currEncod2 = (-1)*(currEncod2);
        if (prevEncod2 == FIRST_VALUE)
        {
            prevEncod2 = currEncod2;
        }
        int change2  = currEncod2 - prevEncod2;
        
        if (!(change2 < 11000 || change2 > 17000))
        {
            int actual_p2 = change2 / 100;
            
            int power_change = power2 - actual_p2; //positive - increase entry, negative - decrease entry
            uBit.display.scroll(power_change);
            if (power_change != 0)
            {
                if (power2 == 100 && power_change > 0)
                  SetMotorPower(1, power1+power_change);
                else
                  SetMotorPower(2, power2+power_change);
            }
            prevChange2 = change2;
        }
        */
        
  

        

        /*
        7000-8999
        actual_p1 = (int) (change / 100);
        
        else
        {
            perform correction
        }

        */
        
        /*
        if (last_neg1)
        {
            last_neg1 = false;
            prevEncod1 = currEncod1;
            return;
        }*/
        //Anytime negative, set skip to three
        
        
        





        


        /*
        if (!counter)
        {
            if (event.value & 0x08) 
            {
                int actual_p2  = m2State ? 100 : 0;
              int power_change = power2 - actual_p2; //positive - increase entry, negative - decrease entry
              if (power_change != 0)
              {
                if (power2 == 100 && power_change > 0)
                  SetMotorPower(1, power1+power_change);
                else
                  SetMotorPower(2, power2+power_change);
                counter++;
              }
              m_image &= ~(0x04 << 10);
            }
            else
            {
                int actual_p1 = m1State ? 100 : 0;
              int power_change = abs(power1) - actual_p1; //positive - incease entry, negative - decrease entry
              if (power_change != 0)
              {
                if (power1 == -100 && power_change > 0)
                  SetMotorPower(2, power2-power_change);
                else
                  SetMotorPower(1, power1-power_change);
                counter++;
              }
            }
        }
        */
        
        
    }
    
}
