CC = gcc
CFLAGS = -Wall -Wextra -Isrc -Isrc/pr -Isrc/utils -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lcurl -lcjson

SRC = src/main.c src/pr/pr.c src/utils/utils.c
BIN = ghcli

all:
	$(CC) $(SRC) -o $(BIN) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(BIN)
