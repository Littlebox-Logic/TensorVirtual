// Tensor VM - bios/bios_interrupt.c

#include "bios_interrupt.h"
#include "../cpu/x86_cpu.h"

void bios_int_5(void)	// Print Screen.
{
	;
}

// ...

void bios_int_10(void)	// Video Service.
{
	switch (reg->ax >> 8)
	{
		// Display mode control.
		case 0x00:	break;
		case 0x0F:	break;
		case 0x1A:	break;
		// Cursor control.
		case 0x01:	break;
		case 0x02:	break;
		case 0x03:	break;
		// Character/String output.
		case 0x09:	break;
		case 0x0A:	break;
		case 0x0E:	break;
		case 0x13:		// write string. 	-- developing.
			char *string = (char *)malloc(reg->cx + 1);
			memset(string, '\0', reg->cx + 1);
			strncpy(string, (reg->es << 4) + reg->bp;
			text_output(string, 255, 255, 255, false);
			free(string);
			break;
		// Graphics function.
		case 0x0C:	break;
		case 0x0D:	break;
		// Screen control.
		case 0x05:	break;
		case 0x06:	break;
		case 0x07:	break;
		case 0x08:	break;
		// Font/Character set control.
		case 0x11:	break;
		// Other.
		case 0x0B:	;
	}
}
