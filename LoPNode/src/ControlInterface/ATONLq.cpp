
#include "ControlInterfaceCommands.h"

byte controlATONLq()
{
    for (int ix = 0; ix < LOP_MAX_OUT_NEIGHBOURS; ix++)
    {
        int ttl = (int)constrain(LOP_ONL_ALLOCATION_TTL - (millis() - OuterNeighboursList[ix]->last_seen), 0, LOP_ONL_ALLOCATION_TTL);
        if (OuterNeighboursList[ix] != 0 && ttl > 0)
        {
            Serial.print(OuterNeighboursList[ix]->tx_power);
            Serial.print(",");
            Serial.print(OuterNeighboursList[ix]->resourceMask.slot);
            Serial.print(",");
            Serial.print(ttl);
            Serial.print(",");
            Serial.println(OuterNeighboursList[ix]->address);
        }
    }

    return ERROR_NONE;
}