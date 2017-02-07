CC=gcc
CFLAGS=#-Wall

ODIR=obj

LIBS =-lpthread

_OBJ = lab2.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

lab2: $(OBJ)
	$(CC) -o $@.bin $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(ODIR)/*.o *.bin


