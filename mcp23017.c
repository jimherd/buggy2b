/*  MCP23017 library for Arduino
    Copyright (C) 2009 David Pye    <davidmpye@gmail.com
    Modified for use on the PIC 18 series chips (J Herd)

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

#include	"defines.h"

//
// create an I2C data structure for the LCD03 display unit
//
I2C_STRUCT   MCP23017_command;

int     MCP23017_i2cAddress;        // physical I2C address
uint16_t	shadow_GPIO, shadow_IODIR, shadow_GPPU, shadow_IPOL;     // Cached copies of the register values

union {
    uint8_t  value8[2];
    uint16_t value16;
} tmp_data;

/*-----------------------------------------------------------------------------
 * reset
 * Set configuration (IOCON) and direction(IODIR) registers to initial state
 */
void MCP23017_reset() 
{
int  reg_addr;
//
// First make sure that the device is in BANK=0 mode
//
    MCP23017_writeRegister_uint8(0x05, (uint8_t)0x00);
//
// set direction registers to inputs
//
    MCP23017_writeRegister_uint16(IODIR, (uint16_t)0xFFFF);
//
// set all other registers to zero (last of 10 registers is OLAT)
//
    for (reg_addr = 2 ; reg_addr <= OLAT ; reg_addr+=2) {
        MCP23017_writeRegister_uint16(reg_addr, (uint16_t)0x0000);
    }
//
// Set the shadow registers to power-on state
//
    shadow_IODIR = 0xFFFF;
    shadow_GPIO  = 0;
    shadow_GPPU  = 0;
    shadow_IPOL  = 0;
}

/*-----------------------------------------------------------------------------
 * write_bit
 * Write a 1/0 to a single bit of the 16-bit port
 */
void MCP23017_write_bit(int value, int bit_number) {
    if (value == 0) {
        shadow_GPIO &= ~(1 << bit_number);
    } else {
        shadow_GPIO |= 1 << bit_number;
    }
    MCP23017_writeRegister_uint16(GPIO, (uint16_t)shadow_GPIO);
}

/*-----------------------------------------------------------------------------
 * Write a combination of bits to the 16-bit port
 */
void MCP23017_write_mask(uint16_t data, uint16_t mask) {
    shadow_GPIO = (shadow_GPIO & ~mask) | data;
    MCP23017_writeRegister_uint16(GPIO, (uint16_t)shadow_GPIO);
}

/*-----------------------------------------------------------------------------
 * read_bit
 * Read a single bit from the 16-bit port
 */
uint16_t  MCP23017_read_bit(uint16_t bit_number) {
    shadow_GPIO = MCP23017_readRegister(GPIO);
    return  ((shadow_GPIO >> bit_number) & 0x0001);
}

/*-----------------------------------------------------------------------------
 * read_mask
 */
uint16_t  MCP23017_read_mask(uint8_t mask) {
    shadow_GPIO = MCP23017_readRegister(GPIO);
    return (shadow_GPIO & mask);
}

/*-----------------------------------------------------------------------------
 * Config
 * set direction and pull-up registers
 */
void MCP23017_config(uint16_t dir_config, uint16_t pullup_config,  uint16_t polarity_config) {
    shadow_IODIR = dir_config;
    MCP23017_writeRegister_uint16(IODIR, (uint16_t)shadow_IODIR);
    shadow_GPPU = pullup_config;
    MCP23017_writeRegister_uint16(GPPU, (uint16_t)shadow_GPPU);
    shadow_IPOL = polarity_config;
    MCP23017_writeRegister_uint16(IPOL, (uint16_t)shadow_IPOL);
}

/*-----------------------------------------------------------------------------
 * writeRegister
 * write a byte
 */
void MCP23017_writeRegister_uint8(uint8_t regAddress, uint8_t data) {
    char  buffer[2];

    MCP23017_command.cmd[0] = MCP23017_i2cAddress;
    MCP23017_command.cmd[1] = regAddress;
    MCP23017_command.cmd[2] = data;
    MCP23017_command.send_count = 3;
    MCP23017_command.get_count = 0;
    MCP23017_command.delay = 0;
    exec_command(&MCP23017_command, WRITE_ONLY);


//    buffer[0] = regAddress;
//    buffer[1] = data;
//    _i2c.write(MCP23017_i2cAddress, buffer, 2);
}

/*----------------------------------------------------------------------------
 * write Register
 * write two bytes
 */ 
void MCP23017_writeRegister_uint16(uint8_t regAddress, uint16_t data) 
{
char  buffer[3];

    MCP23017_command.cmd[0] = MCP23017_i2cAddress;
    MCP23017_command.cmd[1] = regAddress;
    MCP23017_command.cmd[2] = tmp_data.value8[0];
    MCP23017_command.cmd[3] = tmp_data.value8[1];
    MCP23017_command.send_count = 4;
    MCP23017_command.get_count = 0;
    MCP23017_command.delay = 0;
    exec_command(&MCP23017_command, WRITE_ONLY);

//    buffer[0] = regAddress;
//    tmp_data.value16 = data;
//    buffer[1] = tmp_data.value8[0];
//    buffer[2] = tmp_data.value8[1];
//   _i2c.write(MCP23017_i2cAddress, buffer, 3);
}

/*-----------------------------------------------------------------------------
 * readRegister
 */
int MCP23017_readRegister(uint8_t regAddress) {
    char buffer[2];

    MCP23017_command.cmd[0] = MCP23017_i2cAddress;
    MCP23017_command.cmd[1] = regAddress;
    MCP23017_command.send_count = 2;
    MCP23017_command.get_count = 1;
    MCP23017_command.delay = 0;
    exec_command(&MCP23017_command, WRITE_ONLY);

//    buffer[0] = regAddress;
//    _i2c.write(MCP23017_i2cAddress, buffer, 1);
//    _i2c.read(MCP23017_i2cAddress, buffer, 2);

    return ((int)(buffer[0] + (buffer[1]<<8)));
}

/*-----------------------------------------------------------------------------
 * pinMode
 */
void MCP23017_pinMode(int pin, int mode) {
    if (DIR_INPUT) {
        shadow_IODIR |= 1 << pin;
    } else {
        shadow_IODIR &= ~(1 << pin);
    }
    MCP23017_writeRegister_uint16(IODIR, (uint16_t)shadow_IODIR);
}

/*-----------------------------------------------------------------------------
 * digitalRead
 */
int MCP23017_digitalRead(int pin) {
    shadow_GPIO = MCP23017_readRegister(GPIO);
    if ( shadow_GPIO & (1 << pin)) {
        return 1;
    } else {
        return 0;
    }
}

/*-----------------------------------------------------------------------------
 * digitalWrite
 */
void MCP23017_digitalWrite(int pin, int val) 
{
uint8_t   isOutput;

    //If this pin is an INPUT pin, a write here will
    //enable the internal pullup
    //otherwise, it will set the OUTPUT voltage
    //as appropriate.
    isOutput = !(shadow_IODIR & 1<<pin);

    if (isOutput) {
        //This is an output pin so just write the value
        if (val) shadow_GPIO |= 1 << pin;
        else shadow_GPIO &= ~(1 << pin);
        MCP23017_writeRegister_uint16(GPIO, (uint16_t)shadow_GPIO);
    } else {
        //This is an input pin, so we need to enable the pullup
        if (val) {
            shadow_GPPU |= 1 << pin;
        } else {
            shadow_GPPU &= ~(1 << pin);
        }
        MCP23017_writeRegister_uint16(GPPU, (uint16_t)shadow_GPPU);
    }
}

/*-----------------------------------------------------------------------------
 * digitalWordRead
 */
uint16_t MCP23017_digitalWordRead(void) {
    shadow_GPIO = MCP23017_readRegister(GPIO);
    return shadow_GPIO;
}

/*-----------------------------------------------------------------------------
 * digitalWordWrite
 */
void MCP23017_digitalWordWrite(uint16_t w) {
    shadow_GPIO = w;
    MCP23017_writeRegister_uint16(GPIO, (uint16_t)shadow_GPIO);
}

/*-----------------------------------------------------------------------------
 * inputPolarityMask
 */
void MCP23017_inputPolarityMask(uint16_t mask) {
    MCP23017_writeRegister_uint16(IPOL, mask);
}

/*-----------------------------------------------------------------------------
 * inputoutputMask
 */
void MCP23017_inputOutputMask(uint16_t mask) {
    shadow_IODIR = mask;
    MCP23017_writeRegister_uint16(IODIR, (uint16_t)shadow_IODIR);
}

/*-----------------------------------------------------------------------------
 * internalPullupMask
 */
void MCP23017_internalPullupMask(uint16_t mask) {
    shadow_GPPU = mask;
    MCP23017_writeRegister_uint16(GPPU, (uint16_t)shadow_GPPU);
}

