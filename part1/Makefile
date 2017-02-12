CC=gcc
CFLAGS=-Wall -Werror -Wmissing-prototypes -g

ODIR=obj

LDLIBS =-lpthread

_OBJ = lab2.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

lab2: $(OBJ)
	$(CC) -o $@.bin $^ $(CFLAGS) $(LDLIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR)/*.o *.bin


