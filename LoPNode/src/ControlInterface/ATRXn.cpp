
#include "ControlInterfaceCommands.h"

byte controlATRXn(bool enabled)
{    
    rx_notification_enable = enabled;
    
    return ERROR_NONE;
}