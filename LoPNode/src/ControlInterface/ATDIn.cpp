
#include "ControlInterfaceCommands.h"

byte controlATDIn(bool enabled)
{
    lop_dia_enabled = enabled;
    
    return ERROR_NONE;    
}