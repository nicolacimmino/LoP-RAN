
#include "ControlInterfaceCommands.h"

byte controlATPWq()
{   
    Serial.println(inbound_tx_power);
    
    return ERROR_NONE;
}