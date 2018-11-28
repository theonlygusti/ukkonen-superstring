IDIR=.
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj

_DEPS=superstring.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

OBJ=main.o


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	make clean

.PHONY: clean

clean:
	rm -f *.o *~ core $(INCDIR)/*~
