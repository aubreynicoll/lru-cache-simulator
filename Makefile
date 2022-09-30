#
# Note: requires a 64-bit x86-64 system 
#
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -m64

cache-simulator: cache-simulator.c 
	$(CC) $(CFLAGS) -o cache-simulator cache-simulator.c  -lm 

#
# Clean the src dirctory
#
clean:
	rm -f cache-simulator
