IDIR=.
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj

_DEPS=superstring.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

OBJ=main.o

EXE=main

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $(EXE) $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(EXE) *.o *~ core $(INCDIR)/*~
