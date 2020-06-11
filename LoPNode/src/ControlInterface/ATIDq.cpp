
#include "ControlInterfaceCommands.h"

byte controlATIDq()
{
    Serial.println("LoP-RAN RadioFW 0.1");

    // The below chunk of code calculates amount of free memory.
    // This code is found in several forums and blogs but I never found
    //  an attribution for it, so I cannot give proper credit.
    // To uderstand what goes on here we need to remember that the stack
    //  grows from the bottom of memory up and the heap from the top down.
    // The code creates an int, which will be created on the stack as it is
    //  a local variable, so the address of v will be the current top of the
    //  stack. It will then subtract this address from the current highest
    //  heap address that is found in the system variable __brkval unless
    //  the heap is empty in which case it will be __heap_start.
    extern int __heap_start, *__brkval;
    int v;
    Serial.print((int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval), DEC);
    Serial.println(" bytes free.");

    return ERROR_NONE;
}