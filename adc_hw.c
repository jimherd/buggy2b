//
// adc_hw.c : Routines to control use A/D unit
// ========
//
//************************************************************************
//
#include      "defines.h"

//************************************************************************
// Function prototypes
//
UINT16  ADC_Read(UINT8 channel);
//
//************************************************************************
//
// System functions : code.
//
//************************************************************************
// BusyADC : check conversion status
// =======
//
// Description
//    This routine checks the GO bit to see if the A/D conversion is done. 
// Notes
//   Return Value:   char: 1 - A/D busy, 0 - Done converting
//   Description:    
//
char BusyADC(void)
{
    return(ADCON0bits.GO);
}

//************************************************************************
// CloseADC : check conversion status
// ========
//
//Description
//   This routine turns the A/D off and disables the A/D interrupt 
//
void CloseADC(void)
{
    ADCON0bits.ADON = 0;
    PIE1bits.ADIE = 0;
}

//************************************************************************
// ConvertADC : start conversion
// ========== 
//
//Description
//  This routine starts an A/D conversion by setting the GO bit.
//
void ConvertADC(void)
{
    ADCON0bits.GO = 1;    // Set the GO bit to start a conversion
}

//************************************************************************
// OpenADC : initialise A/D subsystem
// ======= 
//
//Description
//    This routine first resets the A/D registers  to the POR state and then 
//    configures the A/D clock, interrupt, justification, vref  source, 
//    and I/O.  It also sets the channel.
//
void OpenADC( unsigned char config, 
              unsigned char config2, 
              unsigned char portconfig)
{
    // Reset A/D Registers to POR state
    ADCON0 = 0;
    ADCON1 = 0;
    ADCON2 = 0;

    ADCON0 = (config2 >> 1) & 0b00111100;     // Analog channel selection
    ADCON1 = portconfig;                      // Port configuration

    ADCON1 |= (config2 << 4) & 0b00110000;    // Voltage reference configuration

    ADCON2 = config & 0b10000000;             // Result format
    ADCON2 |= (config >> 4) & 0b00000111;     // Conversion clock select
    ADCON2 |= (config << 2) & 0b00111000;     // Acquisition time select

    if( config2 & 0b10000000 )   // Test the interrupt on/off
    {
      PIR1bits.ADIF = 0;         // Clear the A/D interrupt flag
      PIE1bits.ADIE = 1;         // Enable the A/D interrupt
      INTCONbits.PEIE = 1;       // Enable peripheral interrupts
    }

    ADCON0bits.ADON = 1;         // Enable the A/D
}

//************************************************************************
// ADC_Read : read value from A/D subsystem
// ========
//
//Description
//    Reads the ADRESL and ADRESH and returns these as a long.
//
UINT16  ADC_Read(UINT8 channel)
{
union ADCResult i; 

//
// set channel, set GO, and check for completion
//
    ADCON0 = ((channel << 2) & 0b00111100) | 0b00000001;
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO != 0) {
        ;
    }
//
// read 10-bit value
// 
    i.br[0] = ADRESL;                       // Read ADRESL into the lower byte
    i.br[1] = ADRESH;                    // Read ADRESH into the high byte

    return (i.lr);     // Return the long variable
}

//************************************************************************
// SetChanADC : Select A/D channel
// ==========
//
//Description
//    sets the A/D to the specified channel     
//
void SetChanADC(unsigned char channel)
{
    ADCON0 = ((channel >> 1) & 0b00111100) | (ADCON0  & 0b11000011);

}
