default:
	gcc -Wall -Wextra -Wpedantic -Werror -O2 main.c crc.c -o crc
lax:
	gcc -O2 main.c crc.c -o crc

