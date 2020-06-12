/*
#include <stdio.h>
#include <Python/Python.h>
//#include <MicroBit.h>

//extern MicroBit uBit;


int main()
{
    //uBit.display.scrollAsync(reply);
    char filename[] = "test.py";
    FILE* fp;

    Py_Initialize();


    fp = fopen(filename, "r");
    PyRun_SimpleFile(fp, filename);

    Py_Finalize();
    return 0;
}
*/
/*
    9 Actions
    1 state in, 9 actions and their probabilities
        //Two ways: Choose based on probability and choose max porbability
    //Seed is for the level you are using ()
    //

*/