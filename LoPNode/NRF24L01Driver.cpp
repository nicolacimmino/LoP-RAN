// NR24L01 Driver is part of LoP-RAN , provides control for NRF24L01+ chips.
// This is a custom development of a driver for the NRF24L01+ cips, we use
//  here this chip in particular ways that are not the common application
//  scenarios for them. This is needed as LoP-RAN must be deployable also
//  on different bands where radios with these capabilities are not avaialbe.
// We basically don't make use of any of the automatic retrasmission capabilities
//  since we need to strictly know radio times in order to meet the strict
//  real-time requirements of LoP-RAN TDMA. We also use NRF24L01 "Pipe addresses"
//  as a sort of extended preamble to prevent interslot data leackage.
//
// The code in this module draws some inspiration from the great RF24 
//  library Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>, GNU
//  which served also as a good support during the first phases of tests 
//  to get things up and running.
//
//  Copyright (C) 2014 Nicola Cimmino
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see http://www.gnu.org/licenses/.
//
#include "NRF24L01Driver.h"

// Buffer used to send and receive data on the SPI.
uint8_t spi_buffer[NRF24L01_SPI_BUFFER_SIZE];

///////////////////////////////////////////////////////////////////////////////////////////////
// Brings up the SPI interface and makes basic settings for to the radio.
//
void initializeRadio()
{
    // Initialize SPI related pins.
    pinMode(NRF24L01_CE_PIN, OUTPUT);
    pinMode(NRF24L01_CS_PIN, OUTPUT);
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, OUTPUT);
    digitalWrite(NRF24L01_CS_PIN, HIGH);  
    digitalWrite(NRF24L01_CE_PIN, LOW);
    
    // NRF24L01 PON (Power On Reset) is 10.3mS
    //  according to datasheet. We wait a bit more
    //  just to be on the safe side.
    delay(15);

    // We setup SPI with mode 0, MSB first and use SPI CLOCK
    //  div by 2 to get an 8MHz SPI clock on a 16MHz board).
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV2);

    // Disable Auto ACK.
    writeNRF24L01Register(NRF24L01_EN_AA, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Powers down the radio, the SPI interface is still active.
//
void powerDownRadio()
{
  // Power is controlled by bit 1 of CONFIG
  writeNRF24L01Register(NRF24L01_CONFIG, (readNRF24L01Register(NRF24L01_CONFIG) & 0b11111101)); 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Powers up the radio.
//
void powerUpRadio()
{
  // Power is controlled by bit 1 of CONFIG
  writeNRF24L01Register(NRF24L01_CONFIG, (readNRF24L01Register(NRF24L01_CONFIG) | 0b00000010)); 
}


///////////////////////////////////////////////////////////////////////////////////////////////
// Sets the extended preamble for TX.
//
// In NRF24L01 we implement this using the pipe address feature, which does exactly the same
//  that is add extra bytes after the preamble that should match for a packet to be accepted.
//
void setTXExtendedPreamble(uint64_t extended_preamble)
{
  // TX Address is 5 bytes long.
  writeNRF24L01Register(NRF24L01_TX_ADDR, extended_preamble, 5); 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sets the extended preamble for RX.
//
// In NRF24L01 we implement this using the pipe address feature, which does exactly the same
//  that is add extra bytes expected after the preamble for a packet to be accepted.
//
void setRXExtendedPreamble(uint64_t extended_preamble)
{
  // TX Address is 5 bytes long.
  writeNRF24L01Register(NRF24L01_RX_ADDR_P1, extended_preamble, 5); 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sets transmit power.
//
void setTrasmitPower(uint8_t power)
{
  // Power is in bit2-1 of RF SETUP
  writeNRF24L01Register(NRF24L01_RF_SETUP, (readNRF24L01Register(NRF24L01_RF_SETUP) & 0b11111001) | ((power & 0b11) << 1)); 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sets RF channel.
//
void setRFChannel(uint8_t channel)
{
  // MAX RF channel is 127
  writeNRF24L01Register(NRF24L01_RF_CH, channel & 0b01111111); 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Writes a multiple bytes register.
//
void writeNRF24L01Register(uint8_t address, uint8_t value, uint8_t len)
{
  // Write out the value, LSB first.
  for(int buf_ix=0; buf_ix<len; buf_ix++)
  {
    spi_buffer[0] = value & 0xFF;
    value = value >> 8;
  }
  performSPITransaction(NRF24L01_W_REGISTER | (address & REGISTER_MASK), len);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Writes a single byte register.
//
void writeNRF24L01Register(uint8_t address, uint8_t value)
{
  spi_buffer[0] = value;
  performSPITransaction(NRF24L01_W_REGISTER | (address & REGISTER_MASK), 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Reads a single byte register.
//
uint8_t readNRF24L01Register(uint8_t address)
{
  performSPITransaction(NRF24L01_W_REGISTER | (address & REGISTER_MASK), 1);
  return spi_buffer[0];
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Writes a command to the device and sends a parameter of the specified length, at the same
//  time it gets the response in the same buffer.
//
uint8_t performSPITransaction(uint8_t command, uint8_t data_len)
{
    // CS low before we can talk.
    digitalWrite(NRF24L01_CS_PIN, LOW);
    
    // First we send the command. We also get out the status register.
    uint8_t status = SPI.transfer(command);
    
    // Then the all buffer.
    for (uint8_t buf_ix=0; buf_ix<data_len; buf_ix++)
    {
	spi_buffer[buf_ix] = SPI.transfer(spi_buffer[buf_ix]);
    }
    
    // Release the SPI bus.
    digitalWrite(NRF24L01_CS_PIN, HIGH);
    
    return status;
}
