CC = gcc
CFLAGS = -Wall -std=c99 -g

tcrypt: tcrypt.o IO.o TDES.o magic.o

TDEStest: TDEStest.o TDES.o magic.o

tcrypt.o: tcrypt.c IO.h TDES.h TDESinternal.h
TDES.o: TDES.c TDES.h TDESinternal.h IO.h magic.h
IO.o: IO.c IO.h
magic.o: magic.c magic.h
TDEStest.o: TDEStest.c TDES.h TDESinternal.h

clean:
	rm -f tcrypt
	rm -f TDEStest
	rm -f output*.txt
	rm -f *.o