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
    //  div by 4 to get a 2MHz SPI clock on a 16MHz xtal).
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV4);
      
    // Disable Auto ACK.
    writeNRF24L01Register(NRF24L01_EN_AA, 0);
    
    // Set correct payload size.
    writeNRF24L01Register(NRF24L01_RX_PW_P1, NRF24L01_PAYLOAD_SIZE);
    
    // Enable only pipe1 RX (we don't need zero cause we don't use ACK).
    writeNRF24L01Register(NRF24L01_EN_RXADDR, 0b00000010);
    
    powerUpRadio();
    
    performSPITransaction(NRF24L01_FLUSH_TX, 0);
    performSPITransaction(NRF24L01_FLUSH_RX, 0);
     
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Enters in receive mode.
// Radio must be already in Standby-I when this is invoked.
//
void startReceiving()
{
  // Flush RX FIFO and clear RX_DR flag (DataReady).
  uint8_t status = performSPITransaction(NRF24L01_FLUSH_RX, 0);
  writeNRF24L01Register(NRF24L01_STATUS, status | 0b01000000);
  
  // Set RX mode (bit 0 of CONFIG goes to one)
  // Since we are in Standby-I we need only 130uS for RX to settle.
  writeNRF24L01Register(NRF24L01_CONFIG, (readNRF24L01Register(NRF24L01_CONFIG) | 0b00000001));
  digitalWrite(NRF24L01_CE_PIN, HIGH);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Back to Standby-I mode.
//
void stopReceiving()
{
  digitalWrite(NRF24L01_CE_PIN, LOW);
}

void readPayload(char *buffer)
{
  // Put the RX_FIFO data into the SPI buffer and then move it to the supplied buffer.	 
  performSPITransaction(NRF24L01_R_RX_PAYLOAD, NRF24L01_PAYLOAD_SIZE); 
  memcpy(buffer, spi_buffer, NRF24L01_PAYLOAD_SIZE); 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Returns true if some data is available in the RX buffer.
//
boolean isDataAvailable()
{
  // RX_DR Data Ready flag is on bit 6 of STATUS
  // We read status by just sending a NOP as it is faster (only one
  //  byte transaction against 2 of the usual read register).
  uint8_t status = performSPITransaction(NRF24L01_NOP, 0);
  uint8_t fifo_status = readNRF24L01Register(NRF24L01_FIFO_STATUS);

  // There is data only if Data Ready and not RX Empty
  if((status & _BV(NRF24L01_STATUS_RX_DR)) && !(fifo_status & _BV(NRF24L01_FIFO_STATUS_RX_EMPTY)))
  {
     // Reset Data Ready.
     writeNRF24L01Register(NRF24L01_STATUS, status | _BV(NRF24L01_STATUS_RX_DR));
     return true; 
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sends the supplied buffer.
// Radio must be already in Standby-I when this is invoked.
//
void transmitBuffer(char *buffer, int length)
{ 
  // Flush the TX buffer.  
  performSPITransaction(NRF24L01_FLUSH_TX, 0);

  // Set TX mode (bit 0 of CONFIG goes to zero)
  // Since we are in Standby-I we need only 130uS for TX to settle.
  writeNRF24L01Register(NRF24L01_CONFIG, (readNRF24L01Register(NRF24L01_CONFIG) & ~_BV(NRF24L01_CONFIG_PRIM_RX)));
  digitalWrite(NRF24L01_CE_PIN, HIGH);
  
  // We are here in Standby-II and will move to TX mode as soon as data is pushed to the TX FIFO.
  // Technically we enter this status after 130uS but we can just start to fill the TX FIFO.
  
  unsigned long last_pll_lock = millis();
  for(int buf_ix=0; buf_ix<length; buf_ix+=NRF24L01_PAYLOAD_SIZE)
  {
    memcpy(spi_buffer, buffer+buf_ix, NRF24L01_PAYLOAD_SIZE);
    performSPITransaction(NRF24L01_W_TX_PAYLOAD, NRF24L01_PAYLOAD_SIZE);
    
    // Wait to push more data if we have filled the FIFO
    // Bit 5 is TX_FIFO_FULL
    while(readNRF24L01Register(NRF24L01_FIFO_STATUS) & _BV(NRF24L01_FIFO_STATUS_TX_FULL))
    {
      delayMicroseconds(100);
    }
  
    // The PLL during TX mode runs in open loop mode, we cannot stay here more than 4mS as the 
    //  oscillator might drift too much. If we go over 4mS we need to enter Standby-II mode
    //  and go back to TX mode in order for the oscillator to lock again. To go to Standby-II
    //  here we pull CE down and wait the TX FIFO to be empty.
    if(millis() - last_pll_lock > 4)
    {
      // Wait for the FIFO to be empty, so we can enter Standby-II
      // Bit 4 is TX_FIFO_EMPTY
      while(!(readNRF24L01Register(NRF24L01_FIFO_STATUS) & 0b00010000))
      {
        delayMicroseconds(100);
      }  
    } 
    
  } 
  
  // Wait for the FIFO to be empty, so we can enter Standby-II
  while(!(readNRF24L01Register(NRF24L01_FIFO_STATUS) & 0b00010000))
  {
    delayMicroseconds(100);
  }
  
  // Back to Standby-I
  writeNRF24L01Register(NRF24L01_CONFIG, (readNRF24L01Register(NRF24L01_CONFIG) & 0b11111110));  
  digitalWrite(NRF24L01_CE_PIN, LOW);  
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
void setTransmitPower(uint8_t power)
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
void writeNRF24L01Register(uint8_t address, uint64_t value, uint8_t len)
{
  // Write out the value, LSB first.
  for(int buf_ix=0; buf_ix<len; buf_ix++)
  {
    spi_buffer[buf_ix] = value & 0xFF;
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
  performSPITransaction(NRF24L01_R_REGISTER | (address & REGISTER_MASK), 1);
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
    if(data_len>0)
    {
      for (uint8_t buf_ix=0; buf_ix<data_len; buf_ix++)
      {
        spi_buffer[buf_ix] = SPI.transfer(spi_buffer[buf_ix]);
      }
    }
    
    // Release the SPI bus.
    digitalWrite(NRF24L01_CS_PIN, HIGH);
    
    return status;
}

