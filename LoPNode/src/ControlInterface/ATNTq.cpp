
#include "ControlInterfaceCommands.h"

byte controlATNTq()
{   
    Serial.println((inner_link_up) ? "1" : "0"); 
    
    return ERROR_NONE;
}