
mcr_test:
	cc -mthumb -c thumb.c
	cc -static -o mcr_test main.c thumb.o

clean:
	rm -f mcr_test thumb.o
