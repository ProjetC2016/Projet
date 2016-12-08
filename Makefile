CC=gcc
CFLAGS=-Wall
EXEC=nim1 nim2 nim3 nim4
NAMEOPTION=OPTION

all: $(EXEC)

nim1 : exo1.c
	$(CC) $(CFLAGS) $^ -o $@ -D $(NAMEOPTION)=playClassic

nim2 : exo1.c
	$(CC) $(CFLAGS) $^ -o $@ -D $(NAMEOPTION)=playRandom

nim3 : exo1.c
	$(CC) $(CFLAGS) $^ -o $@ -D $(NAMEOPTION)=play1

nim4 : exo1.c
	$(CC) $(CFLAGS) $^ -o $@ -D $(NAMEOPTION)=playOptimal


.PHONY: cleanall

cleanall:
	@rm -rf main
	@echo "Cleared"
