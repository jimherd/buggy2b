/* draft mbed TextLCD 
 * (c) 2007/8, sford
 * Modified jherd
 */
#include  "defines.h"
/*
 * Initialisation
 * ==============
 *
 * After attaching the supply voltage/after a reset, the display needs to be brought in to a defined state
 *
 * - wait approximately 15 ms so the display is ready to execute commands
 * - Execute the command 0x30 ("Display Settings") three times (wait 1,64ms after each command, the busy flag cannot be queried now). 
 * - The display is in 8 bit mode, so if you have only connected 4 data pins you should only transmit the higher nibble of each command.
 * - If you want to use the 4 bit mode, now you can execute the command to switch over to this mode now.
 * - Execute the "clear display" command
 *
 * Timing
 * ======
 *
 * Nearly all commands transmitted to the display need 40us for execution. 
 * Exceptions are the commands "Clear Display and Reset" and "Set Cursor to Start Position" 
 * These commands need 1.64ms for execution. These timings are valid for all displays working with an 
 * internal clock of 250kHz. But I do not know any displays that use other frequencies. Any time you 
 * can use the busy flag to test if the display is ready to accept the next command.
 * 
 * _e is kept high apart from calling clock
 * _rw is kept 0 (write) apart from actions that uses it differently
 * _rs is set by the data/command writes
 * 
 * RS = 7
 * RW = 6
 * E  = 5
 * Back light = 4
 * D4 = 0
 * D5 = 1  
 * D6 = 2
 * D7 = 3  
 */

//----------------------------------------------------------------------------
// Global variables relating to the 16*2 LCD display
//
int _rows;
int _columns;
int _row;
int _column; 
  
//----------------------------------------------------------------------------
// TextLCD_init : initialise display hardware
// ============
//
void TextLCD_init(void) 
{
uint8_t  i;
//
// Initialise pointer to MCP23017 object
//
	DelayMs(1);
    MCP23017_config(0x0F00, 0x0F00, 0x0F00);
	DelayMs(1);    
    _rows = 2;
    _columns = 16;

    TextLCD_rw(0);
    TextLCD_e(0);
    TextLCD_rs(0); // command mode
    
    DelayMs(DISPLAY_INIT_DELAY_MSECS);     
//
// interface defaults to an 8-bit interface. However, we need to ensure that we
// are in 8-bit mode. This code was included in the original MBED code. 
//   
    for(i=0; i<4; i++) {
        TextLCD_writeNibble(CMD4_SET_8_BIT_INTERFACE);
        DelayBigUs(1640);      // this command takes 1.64ms, so wait for it
    }
//
// Above code replaced with the following. Suggested by OLED controller documentation.
// It does not seem to have advers effects on LCD controllers.  The 5 null commands
// reset the controller.
// 
    TextLCD_writeNibble(CMD4_SET_4_BIT_INTERFACE);       // 0x02 - now force into 4-bit mode   
    DelayMs(5); 
       
    TextLCD_writeNibble(CMD_NULL); DelayMs(5);
    TextLCD_writeNibble(CMD_NULL); DelayMs(5);
    TextLCD_writeNibble(CMD_NULL); DelayMs(5);
    TextLCD_writeNibble(CMD_NULL); DelayMs(5);
    TextLCD_writeNibble(CMD_NULL);  
    
    DelayMs(DISPLAY_INIT_DELAY_MSECS);    
      
    TextLCD_writeNibble(CMD4_SET_4_BIT_INTERFACE);       // 0x02 - now force into 4-bit mode   
    DelayMs(5); 
        
    TextLCD_writeCommand(CMD_FUNCTION_SET | INTERFACE_4_BIT | TWO_LINE_DISPLAY | FONT_5x8 | ENGL_JAPAN_FONT_SET); // 0x28
    TextLCD_writeCommand(CMD_DISPLAY_CONTROL | DISPLAY_ON | CURSOR_OFF | CURSOR_CHAR_BLINK_OFF);                  // 0x0C
    TextLCD_writeCommand(CMD_RETURN_HOME);                                                                        // 0x02 
    TextLCD_cls();                                                                                                // 0x01
    TextLCD_writeCommand(CMD_ENTRY_MODE | CURSOR_STEP_RIGHT | DISPLAY_SHIFT_OFF );                                // 0x06
    TextLCD_writeCommand(CMD_MODE_POWER | CHARACTER_MODE | INTERNAL_POWER_ON );                                   // 0x17
}
//----------------------------------------------------------------------------
// TextLCD_putchar : output a single character to the current display cursor point
// ===============
//
uint8_t TextLCD_putchar(uint8_t value) {
    if(value == '\n') {
        TextLCD_newline();
    } else {
        TextLCD_writeData(value);
    }
    return value;
}

//----------------------------------------------------------------------------
// TextLCD_put_string : output a null terminated string to the display
// ==================
//
void   TextLCD_putstring(const char* text)
{
char  *ch_pt;

    ch_pt = text;
    while (*ch_pt != '\0') {
        TextLCD_putchar(*ch_pt++);
    }
}

//----------------------------------------------------------------------------
// TextLCD_newline : move cursor to the start of the next line
// ===============
//
void TextLCD_newline(void) 
{
    _column = 0;
    _row++;
    if(_row >= _rows) {
        _row = 0;
    }
    TextLCD_locate(_column, _row); 
}

//----------------------------------------------------------------------------
// int16_to_string : convert INT16 to ASCII string
// ===============
//
uint8_t int16_to_string(char *str, int16_t num) 
{ 
uint16_t   k; 
char    c, *ostr;
uint8_t   flag, ch_count;

    ch_count = 0;
	if (num < 0) { 
		num = -num; 
		*str++ = '-'; 
        ch_count++;
	} 
	k = 10000; 
	ostr = str;
	flag = 0; 
	while (k != 0) { 
		c = num / k; 
		if ((c > 0) || (k == 1) || (flag == 1)) { 
			num %= k; 
			c += '0'; 
			*str++ = c; 
            ch_count++;
			flag = 1; 
		} 
		k /= 10; 
	} 
	*str = '\0'; 
	return ch_count; 
}


//----------------------------------------------------------------------------
// TextLCD_locate : move cursor to the specified (x,y) location
// ==============
//
void TextLCD_locate(uint8_t row, uint8_t column) 
{
int  address;

    if(column < 0 || column >= _columns || row < 0 || row >= _rows) {
        // error("locate(%d,%d) out of range on %dx%d display", column, row, _columns, _rows);
        return;
    }
    
    _row = row;
    _column = column;
    address = CMD_SET_DDRAM_ADDRESS + (_row * 0x40) + _column; // memory starts at 0x80, and internally it is 40 chars per row (only first 16 used)
    TextLCD_writeCommand(address);            
}

//----------------------------------------------------------------------------
// TextLCD_cls : clear the display and set cursor to location (0,0)
// ===========
//
void TextLCD_cls() {
    TextLCD_writeCommand(CMD_CLEAR_DISPLAY);  // 0x01
    DelayMs(DISPLAY_CLEAR_DELAY);        // 
    TextLCD_locate(0, 0);
}

//----------------------------------------------------------------------------
// TextLCD_reset : call to TextLCD_cls
// =============
//
void TextLCD_reset(void) {
    TextLCD_cls();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Internal low level routines
//----------------------------------------------------------------------------
// TextLCD_clock : generate a 40uS clock pulse for the display
// =============
//
void TextLCD_clock(void) {
    DelayUs(40);
    TextLCD_e(1);
    DelayUs(40);
    TextLCD_e(0);    
}

//----------------------------------------------------------------------------
// TextLCD_writeNibble : write a 4-bit value to the display
// ===================
//
void TextLCD_writeNibble(uint8_t value) {

    TextLCD_d(value); 
        DelayUs(40);     // most instructions take 40us
    TextLCD_clock();
}

//----------------------------------------------------------------------------
// TextLCD_writeByte : write an 8-bit value to the display
// =================
//
void TextLCD_writeByte(uint8_t value) {
    TextLCD_writeNibble((value >> 4) & 0x000F);
    TextLCD_writeNibble((value >> 0) & 0x000F);
}

//----------------------------------------------------------------------------
// TextLCD_writeCommand : write an 8-bit command to the display
// ====================
//
void TextLCD_writeCommand(uint8_t command) {
    TextLCD_rs(0);
    TextLCD_writeByte(command);
    DelayMs(DISPLAY_CMD_DELAY);
}

//----------------------------------------------------------------------------
// TextLCD_writeData : write an 8-bit data value to the display
// =================
//
void TextLCD_writeData(uint8_t data) {
    TextLCD_rs(1);
    TextLCD_writeByte(data);
    _column++;
    if(_column >= _columns) {
        TextLCD_newline();
    } 
}

//----------------------------------------------------------------------------
// TextLCD_rs : write a 1-bit value to the display RS line
// ==========
//
void TextLCD_rs(int data) {
    MCP23017_write_bit(data, RS_BIT);
}

//----------------------------------------------------------------------------
// TextLCD_rs : write a 1-bit value to the display RW line
// ==========
//
void TextLCD_rw(int data) {
    MCP23017_write_bit(data, RW_BIT);
}

//----------------------------------------------------------------------------
// TextLCD_e : write a 1-bit value to the display enable line
// =========
//
void TextLCD_e(int data) {
    MCP23017_write_bit(data, E_BIT);
}

//----------------------------------------------------------------------------
// TextLCD_d : write a 1-bit value to the display 'd' line
// ==========
//
void TextLCD_d(int data) {
    MCP23017_write_mask((unsigned short)data, (unsigned short)0x000F);
}
