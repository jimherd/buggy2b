//
// LCD03.c : Routines to control LCD03 4*20 character LCD display
//
//************************************************************************
//
#include      "defines.h"
//
// create an I2C data structure for the LCD03 display unit
//
I2C_STRUCT   LCD_command;
//
//************************************************************************
// System functions : code.
//************************************************************************
//
//************************************************************************
//
// LCD03_Write_Cmd : send a command to the LCD03 display
// ===============
//
void LCD03_Write_Cmd(uint8_t command)
{
    LCD_command.cmd[0] = LCD03_ADDRESS;
    LCD_command.cmd[1] = REGISTER_0;
    LCD_command.cmd[2] = command;
    LCD_command.send_count = 3;
    LCD_command.get_count = 0;
    LCD_command.delay = 0;
    exec_command(&LCD_command, WRITE_ONLY);
}

//************************************************************************
//
// LCD03_Write_Char : output a single text character to the display
// ================
//
void LCD03_Write_Char(char letter)
{
    LCD03_Write_Cmd(letter);
}

//************************************************************************
//
// LCD03_Write_String : output a single text character to the display
// ==================
//
void   LCD03_Write_String(const char text[])
{
char  *ch_pt;

    ch_pt = &text[0];
    while (*ch_pt) {
        LCD03_Write_Char(*ch_pt++);
    }
}

//************************************************************************
//
// LCD03_Set_Cursor : set display cursor.
// ================
//
//  Inputs
//    row     :  value 1 to 4.
//    column  :  value 1 to 20
//
void   LCD03_Set_Cursor(uint8_t row, uint8_t column)
{
    LCD_command.cmd[0] = LCD03_ADDRESS;
    LCD_command.cmd[1] = REGISTER_0;
    LCD_command.cmd[2] = SET_ROW_COL_CURSOR;
    LCD_command.cmd[3] = row;
    LCD_command.cmd[4] = column;
    LCD_command.send_count = 5;
    LCD_command.get_count = 0;
    LCD_command.delay = 0;
    exec_command(&LCD_command, WRITE_ONLY);
}
