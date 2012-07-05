//
// delay.c : delay routines.
//
#include  "defines.h"
//
// high level delay routines - see delay.h for more info.
// Designed by Shane Tolmie of KeyGhost corporation.  Freely distributable.
// Questions and comments to shane@keyghost.com
// PICuWEB - Program PIC micros with C. Site has FAQ and sample source code. http://www.workingtex.com/htpic
// For Microchip PIC18xxxxx and Hi-Tech C
//

#if PIC_CLK == 40000000

#define     WaitFor1Us      Delay10TCY()

    void DelayUs(unsigned char uS_value) {
        Delay10TCYx(uS_value);
    }
 
#elif PIC_CLK == 20000000

#define WaitFor1Us Delay1TCY();Delay1TCY();Delay1TCY();Delay1TCY();Delay1TCY()
    void DelayUs(unsigned char uS_value) {
        Delay10TCYx(uS_value >> 1);
    }
#else
	#error delay.h - please define PIC_CLK correctly
#endif

void CLRWDT(void) {
}

void DelayBigUs(unsigned int cnt) {
unsigned char	i;

	i = (unsigned char)(cnt>>8);
	while(i>=1) {
		i--;
		DelayUs(253);
		CLRWDT();
	}
	DelayUs((unsigned char)(cnt & 0xFF));
}

void DelayMs(unsigned char cnt) {
unsigned char	i;
	do {
		i = 4;
		do {
			DelayUs(250);
			CLRWDT();
		} while(--i);
	} while(--cnt);
}

//
// this copy is for the interrupt function
//
void DelayMs_interrupt(unsigned char cnt) {
unsigned char	i;
	do {
		i = 4;
		do {
			DelayUs(250);
		} while(--i);
	} while(--cnt);
}

void DelayBigMs(unsigned int cnt) {
unsigned char	i;
	do {
		i = 4;
		do {
			DelayUs(250);
			CLRWDT();
		} while(--i);
	} while(--cnt);
}

void DelayS(unsigned char cnt) {
	unsigned char i;
	do {
		i = 4;
		do {
			DelayMs(250);
			CLRWDT();
		} while(--i);
	} while(--cnt);
}
