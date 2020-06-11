
#include "ControlInterfaceCommands.h"

byte controlATTX()
{
    uint8_t address = atoi(strtok(NULL, " "));
    char *message = strtok(NULL, "\n");

    return sendMessage(address, message, strlen(message));    
}