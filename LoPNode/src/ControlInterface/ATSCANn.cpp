
#include "ControlInterfaceCommands.h"

byte controlATSCANn(bool enabled)
{
    scanner_mode = enabled;
    
    return ERROR_NONE;    
}