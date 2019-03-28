CFLAGS=-O3
OUTPUT=hexdumper
SRC=hexdumper.c

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)
