CC = cc
CFLAGS = -std=c99
CFLAGS_PROXY = -lpthread
OBJ = ./obj/
SRC = ./src/


# BUILD ALL
all: server.out client.out proxy.out


# BUILD INDIVIDUAL
server.out:
	$(CC) $(CFLAGS) -o $(OBJ)server.out $(SRC)serverloop.c $(SRC)servermain.c $(SRC)error.c $(SRC)myfuncs.c $(SRC)myreadline.c $(SRC)myrwfile.c $(SRC)mywriten.c $(SRC)wraplib.c $(SRC)wrapsock.c $(SRC)wrapstdio.c $(SRC)wrapunix.c

client.out:
	$(CC) $(CFLAGS) -o $(OBJ)client.out $(SRC)clientloop.c $(SRC)clientmain.c $(SRC)error.c $(SRC)myfuncs.c $(SRC)myreadline.c $(SRC)myrwfile.c $(SRC)mywriten.c $(SRC)wraplib.c $(SRC)wrapsock.c $(SRC)wrapstdio.c $(SRC)wrapunix.c

proxy.out:
	$(CC) $(CFLAGS) -o $(OBJ)proxy.out $(SRC)mycache.c $(SRC)mycachetable.c $(SRC)proxyclientloop.c $(SRC)proxymain.c $(SRC)wrappthread.c $(SRC)error.c $(SRC)myfuncs.c $(SRC)myreadline.c $(SRC)myrwfile.c $(SRC)mywriten.c $(SRC)wraplib.c $(SRC)wrapsock.c $(SRC)wrapstdio.c $(SRC)wrapunix.c $(CFLAGS_PROXY)


# CLEAN OBJECT FILES
cleanall:
	rm -f $(OBJ)*.o *.out

cleanobj:
	rm -f $(OBJ)*.o
