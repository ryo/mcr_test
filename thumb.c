#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

 __aligned(8192)
void
thumb_mcr(void)
{
	asm(".rept 4095; nop; .endr");
	asm("mcr p15, 0, r0, c7, c5, 4");
	asm(".rept 4095; nop; .endr");
}

 __aligned(8192)
void
thumb_nop(void)
{
	asm(".rept 4095; nop; .endr");
	asm("nop.w");
	asm(".rept 4095; nop; .endr");
}

__aligned(8192)
void
thumb_ill(void)
{
	asm(".rept 4095; nop; .endr");
	asm(".byte 0xb7,0xb7,0xb7,0xb7");	// 16bit Illegal instruction * 2
	asm(".rept 4095; nop; .endr");
}
