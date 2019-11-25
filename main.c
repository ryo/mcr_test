#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

void thumb_mcr(void);
void thumb_nop(void);
void thumb_ill(void);

static void
dumpstr(const char *data, size_t len)
{
	char ascii[17];
	size_t i;

	ascii[16] = '\0';
	for (i = 0; i < len; i++) {
		unsigned char c;
		if ((i & 15) == 0)
			printf("%08p:", data);
		c = *data++;
		printf(" %02x", c);
		ascii[i & 15] = (0x20 <= c && c <= 0x7e) ? c : '.';
		if ((i & 15) == 15)
			printf(" <%s>\n", ascii);
	}
	ascii[len & 15] = '\0';
	if (len & 15) {
		printf("%*s <%s>\n", 48 - (((int)len & 15) * 3), "", ascii);
	}
}

void
usage(void)
{
	fprintf(stderr, "mcr_test <test>\n");
	fprintf(stderr, "	mcr		execute \"mcr p15, 0, r0, c7, c5, 4\" on arm and thumb mode\n");
	fprintf(stderr, "	mcr_pageout	execute \"mcr p15, 0, r0, c7, c5, 4\" on page boundary (mapped and pageout'ed)\n");
	fprintf(stderr, "	mcr_unmap	execute \"mcr p15, 0, r0, c7, c5, 4\" on page boundary (mapped and unmapped)\n");
	fprintf(stderr, "	nop_pageout	execute \"nop.w\" on page boundary (mapped and pageout'ed)\n");
	fprintf(stderr, "	ill		execute \".insn 0xb7b7\" on page boundary (mapped and unmapped)\n");
	exit(1);
}

int
main(int argc, char *argv[])
{
	int rc;
	int pagesize = 8192;
	uint32_t *insn;
	void *thumb_mcr_addr;
	void *thumb_nop_addr;
	void *thumb_ill_addr;

	if (argc != 2)
		usage();

	setbuf(stdout, NULL);

	printf("pagesize=%d\n", pagesize);


	if (strcmp(argv[1], "mcr") == 0) {
		printf("do mcr insn on 32bit\n");
		asm("mcr p15, 0, r0, c7, c5, 4");
		printf("ok\n\n");

		printf("do mcr insn on thumbmode\n");
		thumb_mcr();
		printf("ok\n\n");
	}


	if (strcmp(argv[1], "mcr_unmap") == 0 ||
	    strcmp(argv[1], "mcr_pageout") == 0) {

		thumb_mcr_addr = (void *)((uintptr_t)thumb_mcr & -2);

		/* dump around page boundary */
		printf("thumb_mcr_addr=%p\n", thumb_mcr_addr);
		dumpstr(thumb_mcr_addr + pagesize - 32, 64);
		printf("\n");

		if (strcmp(argv[1], "mcr_unmap") == 0) {
			/* unmap page (last half of thumb_mcr()) */
			printf("munmap page %p-%p", thumb_mcr_addr + pagesize, thumb_mcr_addr + pagesize * 2 - 1);
			rc = munmap(thumb_mcr_addr + pagesize, pagesize);
			printf(" -> %d\n\n", rc);
		}

		if (strcmp(argv[1], "mcr_pageout") == 0) {
			/* unmap page (last half of thumb_mcr()) */
			printf("pageout %p-%p", thumb_mcr_addr + pagesize, thumb_mcr_addr + pagesize * 2 - 1);
			rc = madvise(thumb_mcr_addr + pagesize, pagesize, MADV_DONTNEED);
			printf(" -> %d", rc);
			rc = madvise(thumb_mcr_addr + pagesize, pagesize, MADV_FREE);
			printf(",%d\n\n", rc);
			getpid();	/* XXX: any syscall(scheduling) needed to apply? */
		}

		/* segv or pagein->ok */
		printf("mcr on thumbmode on unmapped(pageout'ed) page boundary\n");
		thumb_mcr();
		printf("ok\n\n");
	}


	if (strcmp(argv[1], "nop_pageout") == 0) {
		thumb_nop_addr = (void *)((uintptr_t)thumb_nop & -2);

		/* dump around page boundary */
		printf("thumb_nop_addr=%p\n", thumb_nop_addr);
		dumpstr(thumb_nop_addr + pagesize - 32, 64);
		printf("\n");

		/* pageout page (last half of thumb_nop()) */
		printf("pageout %p-%p", thumb_nop_addr + pagesize, thumb_nop_addr + pagesize * 2 - 1);

		rc = madvise(thumb_nop_addr + pagesize, pagesize, MADV_DONTNEED);
		printf(" -> %d", rc);
		rc = madvise(thumb_nop_addr + pagesize, pagesize, MADV_FREE);
		printf(",%d\n\n", rc);
		getpid();	/* XXX: any syscall(scheduling) needed to apply? */

		/* pagein->ok */
		printf("nop on thumbmode on pageout'ed page boundary\n");
		thumb_nop();
		printf("ok\n\n");
	}


	if (strcmp(argv[1], "ill") == 0) {
		thumb_ill_addr = (void *)((uintptr_t)thumb_ill & -2);

		/* dump around page boundary */
		printf("thumb_ill_addr=%p\n", thumb_ill_addr);
		dumpstr(thumb_ill_addr + pagesize - 32, 64);
		printf("\n");

		/* unmap page (last half of thumb_ill()) */
		printf("munmap page %p-%p", thumb_ill_addr + pagesize, thumb_ill_addr + pagesize * 2 - 1);
		rc = munmap(thumb_ill_addr + pagesize, pagesize);
		printf(" -> %d\n\n", rc);

		/* ILLEGAL INSTRUCTION */
		printf("illegal insn on thumbmode on unmapped page boundary\n");
		thumb_ill();
		printf("ok\n\n");
	}
}
