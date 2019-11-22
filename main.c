#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

void thumb_mcr(void);

int
main(int argc, char *argv[])
{
	int rc;
	int pagesize = 8192;
	uint32_t *insn;
	void *thumb_mcr_addr;

	fprintf(stderr, "pagesize=%d\n", pagesize);

	fprintf(stderr, "mcr on 32bit\n");
	asm("mcr p15, 0, r0, c7, c5, 4");
	fprintf(stderr, "ok\n");

	fprintf(stderr, "mcr on thumbmode\n");
	thumb_mcr();
	fprintf(stderr, "ok\n");

	thumb_mcr_addr = (void *)((uintptr_t)thumb_mcr & -2);
	printf("thumb_mcr_addr=%p\n", thumb_mcr_addr);

	fprintf(stderr, "munmap page %p-%p\n", thumb_mcr_addr + pagesize * 2, thumb_mcr_addr + pagesize * 3);
	rc = munmap(thumb_mcr_addr + pagesize * 2, pagesize);
	fprintf(stderr, "munmap -> %d\n", rc);

//	insn = (uint32_t *)( (uintptr_t)(thumb_mcr + pagesize + pagesize - 2) & -2);
//	printf("insn = %p: %08x\n", insn, *insn);

	fprintf(stderr, "mcr on thumbmode on unmapped page boundary\n");
	thumb_mcr();
	fprintf(stderr, "ok\n");
}
