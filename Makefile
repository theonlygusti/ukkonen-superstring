IDIR=.
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj

_DEPS=superstring.h ahocorasick.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

OBJ=main.o

EXE=shortest_common_superstring

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shortest_common_superstring: $(OBJ)
	$(CC) -o $(EXE) $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(EXE) *.o *~ core $(INCDIR)/*~
