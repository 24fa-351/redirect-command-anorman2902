CC = clang
CFLAGS = -Wall -Wextra -pedantic -std=c11

OUTPUT = redir
SRC = redir.c

# Always use WSL
MAKECMD = wsl
RUNCMD = wsl ./$(OUTPUT)

all:
	$(MAKECMD) $(CC) $(CFLAGS) -o $(OUTPUT) $(SRC)

run: $(OUTPUT)
	@echo ""; echo "Usage: ./redir <inp> <cmd> <out>"; echo "Run manually with: $(RUNCMD)"

clean:
	$(MAKECMD) rm -f $(OUTPUT)
