source = main.c model.c crc.c util.c

default:
	gcc -Wall -Wextra -Wpedantic -Werror -O2 $(source) -o crc
debug:
	gcc -g $(source) -o crc

