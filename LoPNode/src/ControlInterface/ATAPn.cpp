
#include "ControlInterfaceCommands.h"

byte controlATAPn(bool enabled)
{
    if (enabled)
    {
        lop_dap = 0;
        EEPROM.write(EEPROM_RFCH_ACT_AS_SEED, 1);

        return ERROR_NONE;
    }

    lop_dap = 0xFF;
    EEPROM.write(EEPROM_RFCH_ACT_AS_SEED, 0);

    return ERROR_NONE;
}