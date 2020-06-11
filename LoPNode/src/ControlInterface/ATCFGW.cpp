
#include "ControlInterfaceCommands.h"

byte controlATCFGW()
{
    uint16_t address = atoi(strtok(NULL, " "));
    uint8_t value = atoi(strtok(NULL, "\n"));

    EEPROM.write(address, value);

    return ERROR_NONE;
}