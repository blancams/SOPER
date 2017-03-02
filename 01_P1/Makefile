CC = gcc -std=c99
CFLAGS = -Wall
EXE = ejercicio4.1 ejercicio4.2 ejercicio9

all: $(EXE)

.PHONY: clean
clean:
	rm -f *.o $(EXE)
	@echo "## ------------------------------"
	@echo "## Archivos objeto y ejecutables eliminados."
	@echo "## ------------------------------"

$(EXE): %: %.o
	@echo "## ------------------------------"
	@echo "## Generado $@.o a partir de $@.c"
	@echo "## ------------------------------"
	$(CC) $(CFLAGS) -o $@ $@.o
	@echo "## ------------------------------"
	@echo "## Generado ejecutable $@ a partir de $^"
	@echo "## ------------------------------"
