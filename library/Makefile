CFLAGS := -m64 -O2 -g -D_GNU_SOURCE -D_REENTRANT -W -I/usr/local/include
LDFLAGS := -m64 -lm

all: mcontainer.c
	$(CC) $(CFLAGS) -Wall -fPIC -c mcontainer.c
	$(CC) $(CFLAGS) -shared -Wl,-soname,libmcontainer.so.1 -o libmcontainer.so.1.0 mcontainer.o

install: libmcontainer.so.1.0
	cp libmcontainer.so.1.0 /usr/lib/libmcontainer.so.1
	ln -fs /usr/lib/libmcontainer.so.1 /usr/lib/libmcontainer.so
	cp mcontainer.h  /usr/local/include


clean:
	rm -f *.so *.o *.1.0


.PHONY: all clean
