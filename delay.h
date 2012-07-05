//
// delay.h :
//
//  function prototypes
//
#ifndef _DELAY_H
#define _DELAY_H

void DelayUs(unsigned char uS_value);
void DelayBigUs(unsigned int cnt);
void DelayMs(unsigned char cnt);
void DelayMs_interrupt(unsigned char cnt);
void DelayBigMs(unsigned int cnt);
void DelayS(unsigned char cnt);

#endif  // _DELAY_H



