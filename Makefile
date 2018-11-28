IDIR=.
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj

_DEPS=superstring.h ahocorasick.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

OBJ=main.o

EXE=ukksupstr

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ukksupstr: $(OBJ)
	$(CC) -o $(EXE) $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(EXE) *.o *~ core $(INCDIR)/*~
