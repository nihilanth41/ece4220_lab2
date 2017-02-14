CC=gcc
#CC=/usr/local/opt/crosstool/arm-linux/gcc-3.3.4-glibc-2.3.2/bin/arm-linux-gcc
INCLUDE := -I/usr/realtime/include -I/usr/src/linux24/include
CFLAGS=-Wall $(INCLUDE) -O3
ODIR=obj
LDLIBS =-lpthread -L/usr/realtime/lib


_OBJ = lab2.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

lab2: $(OBJ)
	$(CC) -o $@.bin $^ $(CFLAGS) $(LDLIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR)/*.o *.bin


