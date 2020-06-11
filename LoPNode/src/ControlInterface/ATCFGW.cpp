
#include "ControlInterfaceCommands.h"

byte controlATCFGW()
{
    unsigned long address = strtol(strtok(NULL, " "), NULL, 0);
    unsigned long value = strtol(strtok(NULL, "\n"), NULL, 0);

    EEPROM.write(address, value);

    return ERROR_NONE;
}