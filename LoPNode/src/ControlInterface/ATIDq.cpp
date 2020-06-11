
#include "ControlInterfaceCommands.h"
#include "../LopParams.h"
#include "../NRF24L01Driver.h"

byte controlATIDq()
{
    Serial.println("LoP-RAN RadioFW 0.1");
    Serial.println("Radio:");
    Serial.println("  CH LO-HI:  " STR(LOP_LOW_CHANNEL) "-" STR(LOP_HI_CHANNEL));
    Serial.println("  MTU:       " STR(LOP_MTU));
    Serial.println("  BCH:       " STR(BCH_PIPE_ADDR));
    Serial.println("  ACH-I:     " STR(ACH_PIPE_ADDR_IN));
    Serial.println("  ACH-O:     " STR(ACH_PIPE_ADDR_OUT));
    Serial.println("  RTX-GUARD: " STR(LOP_RTXGUARD) " mS");
    Serial.println("  SLOT-DUR:  " STR(LOP_SLOTDURATION) " mS");
    Serial.println("  FRAME-DUR: " STR(LOP_FRAMEDURATION) " mS");
    Serial.println("Network:");
    Serial.println("  ONL-MAX:   " STR(LOP_MAX_OUT_NEIGHBOURS));
    Serial.println("  ONL-TTL:   " STR(LOP_ONL_ALLOCATION_TTL) " mS");
    Serial.print("  LKGCH:     ");
    Serial.println(EEPROM.read(EEPROM_RFCH_INNER_NODE));
    Serial.print("  AP:        ");
    Serial.println(EEPROM.read(EEPROM_RFCH_ACT_AS_SEED));
    Serial.print("  NID:       ");
    for (byte ix = 0; ix < 8; ix++)
    {
        Serial.print(EEPROM.read(EEPROM_NID_BASE + ix));
        Serial.print(".");
    }
    Serial.println((char)0x7F);

    if (inner_link_up)
    {
        Serial.println("  LINK:      UP");
        Serial.print("  NADD:      ");
        Serial.println(node_address);
        Serial.print("  DAP:       ");
        Serial.println(lop_dap);
        Serial.print("  ITXPW:     ");
        switch (inbound_tx_power)
        {
        case NRF24L01_TX_POW_0dBm:
            Serial.println("0 dBm");
            break;
        case NRF24L01_TX_POW_m6dBm:
            Serial.println("-6 dBm");
            break;
        case NRF24L01_TX_POW_m12dBm:
            Serial.println("-12 dBm");
            break;
        case NRF24L01_TX_POW_m18dBm:
            Serial.println("-18 dBm");
            break;
        }
    }
    else
    {
        Serial.println("  LINK:      DOWN");
        Serial.println("  NADD:      ---");
        Serial.println("  DAP:       ---");
        Serial.println("  ITXPW:     ---");
    }

    return ERROR_NONE;
}

#define EEPROM_RFCH_INNER_NODE 0x01  // Last known good RF channel
#define EEPROM_RFCH_ACT_AS_SEED 0x02 // Act as anetwork seed if != 0
#define EEPROM_NID_BASE 0x03         // Base of the Network Identifier (8 bytes up to 0x0A