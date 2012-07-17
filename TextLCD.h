
 
#ifndef WATTBOB_TEXTLCD_H
#define WATTBOB_TEXTLCD_H


#define     RS_BIT      7
#define     RW_BIT      6
#define     E_BIT       5
#define     BL_BIT      4   

//
// Registers and bit definitions for 2*16 character display chip
//
#define     CMD_NULL                0x00

#define     CMD_CLEAR_DISPLAY       0x01

#define     CMD_RETURN_HOME         0x02

#define     CMD_ENTRY_MODE          0x04
#define       CURSOR_STEP_LEFT      0x00
#define       CURSOR_STEP_RIGHT     0x02
#define       DISPLAY_SHIFT_OFF     0x00
#define       DISPLAY_SHIFT_ON      0x01
       
#define     CMD_DISPLAY_CONTROL     0x08
#define       DISPLAY_OFF           0x00
#define       DISPLAY_ON            0x04
#define       CURSOR_OFF            0x00
#define       CURSOR_ON             0x02
#define       CURSOR_CHAR_BLINK_OFF 0x00
#define       CURSOR_CHAR_BLINK_ON  0x01

#define     CMD_CURSOR_SHIFT        0x10
#define       SHIFT_CURSOR_LEFT     0x00
#define       SHIFT_CURSOR_RIGHT    0x04
#define       SHIFT_DISPLAY_LEFT    0x08
#define       SHIFT_DISPLAY_RIGHT   0x0C

#define     CMD_MODE_POWER          0x13
#define       CHARACTER_MODE        0x00
#define       GRAPHICS_MODE         0x08
#define       INTERNAL_POWER_OFF    0x00
#define       INTERNAL_POWER_ON     0x04

#define     CMD_FUNCTION_SET        0x20
#define       ENGL_JAPAN_FONT_SET   0x00
#define       EUROPE_FONT_SET       0x01
#define       ENGL_RUSSIAN_FONT_SET 0x02
#define       FONT_5x8              0x00
#define       FONT_5x10             0x04
#define       ONE_LINE_DISPLAY      0x00
#define       TWO_LINE_DISPLAY      0x08
#define       INTERFACE_4_BIT       0x00
#define       INTERFACE_8_BIT       0x10

#define     CMD_SET_CGRAM_ADDRESS   0x40

#define     CMD_SET_DDRAM_ADDRESS   0x80
//
// nibble commands
//
#define     CMD4_SET_4_BIT_INTERFACE 0x2
#define     CMD4_SET_8_BIT_INTERFACE 0x3
//
// Misc 2*16 character display constants
//
#define     DISPLAY_INIT_DELAY_MSECS    500       // 500mS
#define     DISPLAY_CLEAR_DELAY          10       // 10 mS (spec is 6.2mS)
#define     DISPLAY_CMD_DELAY             5       // delay to allow command to complete

/** Class to access 16*2 LCD display connected to an MCP23017 I/O extender chip
 *
 * Derived from the "stream" class to be able to use methods such as "printf"
 *
 * Example :
 * @code
 * .....
 * #include "MCP23017.h"
 * #include "WattBob_TextLCD.h"
 * .....
 * MCP23017            *par_port;
 * WattBob_TextLCD     *lcd;
 *      .....
 * int main()
 *      par_port = new MCP23017(p9, p10, 0x40);
 *      par_port->config(0x0F00, 0x0F00, 0x0F00);           // configure MCP23017 chip on WattBob       
 *      lcd = new WattBob_TextLCD(par_port);
 *
 *      par_port->write_bit(1,BL_BIT);   // turn LCD backlight ON
 *      lcd->cls(); lcd->locate(0,0);
 *      lcd->printf("%s", message);
 *      lcd->locate(1,0);lcd->printf("press 1 to cont"); 
 * @endcode
 */ 

/** Create TextLCD object connected to a MCP23017 device
 *
 * @param   port    pointer to MCP23017 object
 */ 
void TextLCD_init(void);
    
/** Set cursor to a known point
*
*
* @param   row   integer row number (0 or 1)
* @param   col   integer column number (0 or 15)     
*/            
void TextLCD_locate(uint8_t row, uint8_t column);
    
/** clear display
 *
 * Virtual function for stream class
 */     
void TextLCD_cls(void);

/** reset the display
 *
 * Virtual function for stream class
 */             
void TextLCD_reset();
        

uint8_t TextLCD_putc(uint8_t c);        
int _getc();
void TextLCD_newline(void);      
    
void TextLCD_clock();
void TextLCD_writeData(uint8_t data);
void TextLCD_writeCommand(uint8_t command);
void TextLCD_writeByte(uint8_t value);
void TextLCD_writeNibble(uint8_t value);
    
void TextLCD_rs (int data);
void TextLCD_rw (int data);    
void TextLCD_e (int data);
void TextLCD_d (int data);
          
#endif
