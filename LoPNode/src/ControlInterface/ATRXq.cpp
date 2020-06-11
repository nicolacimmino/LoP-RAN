
#include "ControlInterfaceCommands.h"

byte controlATRXq()
{
    Serial.println(rx_notification_enable ? "1" : "0");

    return ERROR_NONE;
}