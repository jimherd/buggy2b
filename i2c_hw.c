//
// i2c_hw.c : i2c access routines.
//
//

#include  "defines.h"

//************************************************************************
//************************************************************************
// I2C_Open   Open I2C unit
// ========
//
void I2C_Open(uint8_t sync_mode, uint8_t slew )
{
  SSPSTAT &= 0x3F;                // power on state 
  SSPCON1 = 0x00;                 // power on state
  SSPCON2 = 0x00;                 // power on state
  SSPCON1 |= sync_mode;           // select serial mode 
  SSPSTAT |= slew;                // slew rate on/off 

  DDRCbits.RC3 = 1;               // Set SCL (PORTC,3) pin to input
  DDRCbits.RC4 = 1;               // Set SDA (PORTC,4) pin to input

  SSPCON1 |= SSPENB;              // enable synchronous serial port 
}

//************************************************************************
// I2C_Write   Write a single byte
// =========
//
uint8_t I2C_Write( uint8_t data_out )
{
  SSPBUF = data_out;                // write single byte to SSPBUF
    if ( SSPCON1bits.WCOL ) {       // test if write collision occurred
        return ( -1 );              // if WCOL bit is set return negative #
    } else {
        while( SSPSTATbits.BF )     // wait until write cycle is complete  
            ;       
        return ( 0 );               // if WCOL bit is not set return non-negative #
    }
}

//************************************************************************
// I2C_Read   Read a single byte
// ========
//
uint8_t I2C_Read( void )
{
  SSPCON2bits.RCEN = 1;           // enable master for 1 byte reception
  while ( !SSPSTATbits.BF );      // wait until byte received  
  return ( SSPBUF );              // return with read byte 
}

//************************************************************************
// I2C_Idle   Test and wait until I2C module is idle
// =======
//
void I2C_Idle( void )
{
  while ( ( SSPCON2 & 0x1F ) | ( SSPSTATbits.R_W ) )
     continue;
}

//************************************************************************
// I2C_Stop   Send I2C bus stop condition
// ========
//
void I2C_Stop( void )
{
  SSPCON2bits.PEN = 1;            // initiate bus stop condition
}

//************************************************************************
// I2C_Restart   initiate bus restart condition
// ===========
//
void I2C_Restart( void )
{
    SSPCON2bits.RSEN = 1;           // 
}

//************************************************************************
// I2C_Start   Send I2C bus start condition
// ========
//
void I2C_Start( void )
{
    SSPCON2bits.SEN = 1;            // initiate bus start condition
}

//************************************************************************
// I2C_NotAck   
// ==========
//
void I2C_NotAck( void )
{
    SSPCON2bits.ACKDT = 1;          // set acknowledge bit for not ACK
    SSPCON2bits.ACKEN = 1;          // initiate bus acknowledge sequence
}

//************************************************************************
// I2C_Ack   
// =======
//
void I2C_Ack(void)
{
    SSPCON2bits.ACKDT = 0;           // set acknowledge bit state for ACK
    SSPCON2bits.ACKEN = 1;           // initiate bus acknowledge sequence
}

//************************************************************************
// exec_command : execute an I2C cmmand
// ============
//
// Description
//    General purpose I2C communications routine.  An I2C_STRUCT structure
//    provides all the necessary information for a write and optional read
//    transfer.
//
void exec_command(I2C_STRUCT *command, uint8_t mode)
{
uint8_t i, count, address;

	address = ((command->cmd[0] << 1) & I2C_READ_MASK);
	command->cmd[0] = address;
//
// write byte stream
//
    I2C_Idle();
    I2C_Start();
    I2C_Idle();
    for (i=0 ; i<command->send_count ; i++) {
        I2C_Write(command->cmd[i]);
        while(SSPCON2bits.ACKSTAT)
            ;
        I2C_Idle();
    }
//
// if there is no requirement to read any data then just return.
//
	if (mode == WRITE_ONLY) {
    	I2C_Stop();
    	I2C_Idle();
		return;
	}
//
// restart or fresh stop/start
//
    if (mode == MODE_RESTART) {
        I2C_Restart();
        I2C_Idle();
    } else {                        // must be MODE_STOP_START
        I2C_Stop();
        I2C_Idle();
        DelayUs(10);
        if (command->delay != 0) {
            DelayBigMs(command->delay);
        }
        I2C_Start();
        I2C_Idle();
    }
//
// read byte stream
//	
    I2C_Write(command->cmd[0] | I2C_WRITE_MASK);
    while(SSPCON2bits.ACKSTAT)
        ;
    I2C_Idle();
    count = command->get_count - 1;
    for (i=0 ; i < count ; i++) {
            command->reply[i] = I2C_Read(); 
            I2C_Idle();
            I2C_Ack();
            I2C_Idle();
    }
    command->reply[command->get_count-1] = I2C_Read(); 
    I2C_Idle();
    I2C_NotAck();
    I2C_Idle();
    I2C_Stop();
    I2C_Idle();
    return;
}

