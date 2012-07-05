//
// i2c_hw.h : i2c routines
//

#ifndef _I2C_HW_H
#define _I2C_HW_H

#define      MAX_SEND_BYTES    6
#define      MAX_GET_BYTES     6
//
// structure to hold the data for an I2C command
//
typedef struct {
       uint8_t  cmd[MAX_SEND_BYTES];  // command byte list
       uint8_t  send_count;           // number of bytes to be sent
       uint8_t  reply[MAX_GET_BYTES]; // list of byte values received
       uint8_t  get_count;            // number of bytes to be read
       uint32_t delay;                // delay in mS between command write and data read
       uint8_t  status;               // error code or OK
} I2C_STRUCT;

//
// I2C baud rate generator constants
//
#define     I2C_100KHZ          0x62
#define     I2C_400KHZ          0x0E
//
//  I2C SSPCON1 REGISTER
//
#define   SSPENB    0x20  /* Enable serial port and configures
                             SCK, SDO, SDI                      */
#define   SLAVE_7   6     /* I2C Slave mode, 7-bit address      */
#define   SLAVE_10  7     /* I2C Slave mode, 10-bit address     */
#define   MASTER    8     /* I2C Master mode                    */

/* SSPSTAT REGISTER */
#define   SLEW_OFF  0xC0  /* Slew rate disabled for 100kHz mode */
#define   SLEW_ON   0x00  /* Slew rate enabled for 400kHz mode  */
//
// I2C comms mode
//
#define     MODE_STOP_START       0x03
#define     MODE_RESTART          0x0C
#define     WRITE_ONLY            0x30

#define     I2C_READ_MASK          0xFE
#define     I2C_WRITE_MASK         0x01

//************************************************************************
// Function prototypes
//
void   I2C_Open( uint8_t sync_mode, uint8_t slew );
uint8_t  I2C_Write( uint8_t data_out );
uint8_t  I2C_Read( void );
void   I2C_Idle(void);
void   I2C_Stop(void);
void   I2C_Start(void);
void   I2C_Restart(void);
void   I2C_Start(void);
void   I2C_NotAck(void);
void   I2C_Ack(void);

void   exec_command(I2C_STRUCT *command, uint8_t mode);


#endif //_I2C_HW_H
