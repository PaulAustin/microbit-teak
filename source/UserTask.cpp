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

//------------------------------------------------------------------------------
// UserProgramTask - (TODO) A task for running the users saved program.
// Perhaps there are 2 or 3 programs
class UserProgramTask : public TeakTask {
public:
    UserProgramTask();
};
UserProgramTask gUserProgramTask;
TeakTask* gpUserProgramTask = &gUserProgramTask;

UserProgramTask::UserProgramTask()
{
    m_image = PBMAP(
        PBMAP_ROW(1, 1, 1, 1, 1),
        PBMAP_ROW(1, 0, 0, 0, 1),
        PBMAP_ROW(1, 0, 0, 0, 1),
        PBMAP_ROW(1, 0, 0, 0, 1),
        PBMAP_ROW(1, 1, 1, 1, 1),
        PBMAP_FRAME_COUNT(1));
}

//------------------------------------------------------------------------------
// A task to use the built-in level (accelerometer)
class LevelTask  : public TeakTask {
public:
    LevelTask();
};
LevelTask gLevelTask;
TeakTask* gpLevelTask = &gLevelTask;

LevelTask::LevelTask()
{
    m_image = PBMAP(
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 1, 0, 1, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_FRAME_COUNT(1));
}

//------------------------------------------------------------------------------
// A teask to use the built-in temp
class TempTask  : public TeakTask {
public:
    TempTask();
};
TempTask gTempTask;
TeakTask* gpTempTask = &gTempTask;

TempTask::TempTask()
{
    m_image = PBMAP(
        PBMAP_ROW(0, 1, 1, 1, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 1, 0, 0),
        PBMAP_ROW(0, 0, 0, 0, 0),
        PBMAP_FRAME_COUNT(1));
}
