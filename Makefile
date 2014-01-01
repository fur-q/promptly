LUA     = luajit
CFLAGS  = `pkg-config --cflags $(LUA)` -Wall -fPIC -O2 
LDFLAGS = `pkg-config --libs $(LUA)`

promptly: promptly.o 
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	-rm -f promptly promptly.o

