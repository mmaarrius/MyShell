.PHONY: build run clean

build:
	gcc -o myshell main.c builtin.c utils.c -Wall -Wextra -Werror

run: build
	./myshell

clean:
	rm -f myshell