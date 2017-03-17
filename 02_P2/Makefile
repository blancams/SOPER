CC = gcc -std=gnu99
CFLAGS = -Wall
EXE1 = ejercicio2 ejercicio3a ejercicio3b ejercicio4
EXE2 = ejercicio6 ejercicio7 ejercicio8 ejercicio9 ejercicio10

all: $(EXE1) $(EXE2)

.PHONY: clean
clean:
	rm -f *.o $(EXE1) $(EXE2)
	@echo "## ------------------------------"
	@echo "## Archivos objeto y ejecutables eliminados."
	@echo "## ------------------------------"

$(EXE1): %: %.o
	@echo "## ------------------------------"
	@echo "## Generado $@.o a partir de $@.c"
	@echo "## ------------------------------"
	$(CC) $(CFLAGS) -o $@ $@.o -lpthread
	@echo "## ------------------------------"
	@echo "## Generado ejecutable $@ a partir de $^"
	@echo "## ------------------------------"

$(EXE2): %: %.o
	@echo "## ------------------------------"
	@echo "## Generado $@.o a partir de $@.c"
	@echo "## ------------------------------"
	$(CC) $(CFLAGS) -o $@ $@.o
	@echo "## ------------------------------"
	@echo "## Generado ejecutable $@ a partir de $^"
	@echo "## ------------------------------"
