CC?=gcc

# Build the vector shell.
map-cli: map.c cli.c
	$(CC) -o $@ $^
