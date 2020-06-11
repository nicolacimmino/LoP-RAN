
#include "ControlInterfaceCommands.h"

byte controlATIPWq()
{   
    Serial.println(inbound_tx_power);
    
    return ERROR_NONE;
}