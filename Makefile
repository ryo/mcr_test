
mcr_test: main.c thumb.c
	cc -mthumb -c -O -fomit-frame-pointer thumb.c
	cc -static -o mcr_test -O main.c thumb.o

clean:
	rm -f mcr_test thumb.o
