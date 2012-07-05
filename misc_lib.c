//
// misc_lib.c : Utility fuctions
//
//************************************************************************
//
#include      "defines.h"

//************************************************************************
// int16_to_asc : convert INT16 to ASCII string
// ============
//
UINT8 int16_to_asc(char *str, INT16 num) 
{ 
INT16   k; 
char    c, *ostr;
UINT8   flag, ch_count;

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
