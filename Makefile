CC=gcc
CFLAGS=-Wall
EXEC=tchatche tchatche_server

all: $(EXEC)

tchatche : tchatche.c
	$(CC) $(CFLAGS) $^ -o $@

tchatche_server : tchatche_server.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	@rm -rf tchatche tchatche_server
	@echo "Cleared"
