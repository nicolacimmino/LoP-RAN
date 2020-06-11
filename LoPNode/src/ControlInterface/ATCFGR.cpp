
#include "ControlInterfaceCommands.h"

byte controlATCFGR()
{
    unsigned long addressBase = atoi(strtok(NULL, " "));
    unsigned long cnt = atoi(strtok(NULL, "\n"));

    for (uint16_t addressOffset = 0; addressOffset < cnt; addressOffset++)
    {
        uint16_t actualAddress = addressBase + addressOffset;

        byte value = EEPROM.read(actualAddress);

        if (addressOffset % 16 == 0)
        {
            for (int8_t ix = 4; ix >= 0; ix--)
            {
                Serial.print((actualAddress >> (ix * 4)) & 0xF, HEX);
            }
            Serial.print(" ");
        }

        Serial.print(value >> 4, HEX);
        Serial.print(value & 0xF, HEX);

        Serial.print((addressOffset % 16 == 16 - 1) ? "\r\n" : ".");
    }

    Serial.println("");

    return ERROR_NONE;
}