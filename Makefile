CFLAGS  = `pkg-config --cflags luajit` -Wall -O2 -fPIC 
LDFLAGS = `pkg-config --libs luajit`

promptly: promptly.o 
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	-rm -f promptly promptly.o
