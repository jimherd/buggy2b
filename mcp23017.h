/*  MCP23017 library for Arduino
    Copyright (C) 2009 David Pye    <davidmpye@gmail.com
    Modified for use on Heriot-Watt Wattbot Ib robot (J Herd)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef     MBED_MCP23017_H
#define     MBED_MCP23017_H

//****************************************************************
// Register defines from data sheet - we set IOCON.BANK to 0
// as it is easier to manage the registers sequentially.
//****************************************************************

#define     IODIR       0x00
#define     IPOL        0x02
#define     GPINTEN     0x04
#define     DEFVAL      0x06
#define     INTCON      0x08
#define     IOCON       0x0A
#define     GPPU        0x0C
#define     INTF        0x0E
#define     INTCAP      0x10
#define     GPIO        0x12
#define     OLAT        0x14

#define     I2C_BASE_ADDRESS    0x40

#define     DIR_OUTPUT      0
#define     DIR_INPUT       1

//****************************************************************
// Function prototypes
//****************************************************************

/** MCP23017_reset : Reset MCP23017 device to its power-on state
 */    
void MCP23017_reset(void);

/** MCP23017_write_bit : Write a 0/1 value to an output bit
 *
 * @param   value         0 or 1
 * @param   bit_number    bit number range 0 --> 15
 */   
void MCP23017_write_bit(int value, int bit_number);
      
/** MCP23017_write_mask : Write a masked 16-bit value to the device
 *
 * @param   data    16-bit data value
 * @param   mask    16-bit mask value
 */       
void MCP23017_write_mask(uint16_t data, uint16_t mask);

/** MCP23017_read_bit : Read a 0/1 value from an input bit
 *
 * @param   bit_number    bit number range 0 --> 15
 * @return                0/1 value read
 */       
uint16_t  MCP23017_read_bit(uint16_t bit_number);
    
/** MCP23017_read_mask : Read a 16-bit value from the device and apply mask
 *
 * @param   mask    16-bit mask value
 * @return          16-bit data with mask applied
 */     
uint16_t  MCP23017_read_mask(uint8_t mask);

/** MCP23017_config : Configure an MCP23017 device
 *
 * @param   dir_config         data direction value (1 = input, 0 = output)
 * @param   pullup_config      100k pullup value (1 = enabled, 0 = disabled)
 * @param   polarity_config    polarity value (1 = flip, 0 = normal)
 */           
void MCP23017_config(uint16_t dir_config, uint16_t pullup_config, uint16_t polarity_config);

void MCP23017_writeRegister_uint8(uint8_t regAddress, uint8_t  val);
void MCP23017_writeRegister_uint16(uint8_t regAddress, uint16_t val);

int  MCP23017_readRegister(uint8_t regAddress);

/*----------------------------------------------------------------------------- 
 * pinmode
 * Set units to sequential, bank0 mode
 */  
void MCP23017_pinMode(int pin, int mode); 
void MCP23017_digitalWrite(int pin, int val);
int  MCP23017_digitalRead(int pin);

// These provide a more advanced mapping of the chip functionality
// See the data sheet for more information on what they do

//Returns a word with the current pin states (ie contents of the GPIO register)
uint16_t MCP23017_digitalWordRead(void);
// Allows you to write a word to the GPIO register
void MCP23017_digitalWordWrite(uint16_t w);
// Sets up the polarity mask that the MCP23017 supports
// if set to 1, it will flip the actual pin value.
void MCP23017_inputPolarityMask(uint16_t mask);
//Sets which pins are inputs or outputs (1 = input, 0 = output) NB Opposite to arduino's
//definition for these
    void MCP23017_inputOutputMask(uint16_t mask);
// Allows enabling of the internal 100k pullup resisters (1 = enabled, 0 = disabled)
void MCP23017_internalPullupMask(uint16_t mask);
int MCP23017_read(void);
void MCP23017_write(int data);

#endif