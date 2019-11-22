#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

void
thumb_mcr(void)
{
	asm(".align 13");
	asm(".rept 4095; nop; .endr");
	asm("mcr p15, 0, r0, c7, c5, 4");
	asm(".rept 4095; nop; .endr");
}
