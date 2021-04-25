# FIT VUT 2020/2021
# IOS - Projekt 2
# Santa Claus problem
# Author: Lucie Svobodová, xsvobo1x
# Date: 25.04.2021
# Makefile

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread

proj2: proj2.c proj2.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o proj2
