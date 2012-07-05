//
// LCD03.h : header information to allow usage of LCD03 display
//
#ifndef _LCD03_H
#define _LCD03_H

//************************************************************************
// Constant declarations
//************************************************************************
//
// LCD03 I2C addresses
//
#define     LCD03_ADDRESS    0x63
#define     LCD03_READ       0xC7
#define     LCD03_WRITE      0xC6
//
// LCD03 has 4 internal registers
//                                       READ                  WRITE
#define     REGISTER_0     0   // free bytes in buffer     command register
#define     REGISTER_1     1   // Keypad state Low byte          X
#define     REGISTER_2     2   // Keypad state High byte         X
#define     REGISTER_3     3   //      Version                   X
//
// LCD03 registers 0 commands
//
#define     SET_ROW_COL_CURSOR   3
#define     HIDE_CURSOR          4
#define     CLEAR_SCREEN        12
//
//************************************************************************
// System functions : prototypes.
//************************************************************************
//
void   LCD03_Write_Cmd(uint8_t command);
void   LCD03_Write_Char(char letter);
void   LCD03_Write_String(const char text[]);
void   LCD03_Set_Cursor(uint8_t row, uint8_t column);

#endif //_LCD03_H
