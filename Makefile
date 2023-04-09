CC = gcc
CCOPTS = -pthread

dining_philosophers: dining_philosophers.c
	$(CC) $(CCOPTS) -o dp dining_philosophers.c

clean:
	rm -f *.o *.exe