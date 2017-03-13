CC = gcc -std=gnu99
CFLAGS = -Wall
EXE = ejercicio2 ejercicio3a ejercicio3b ejercicio4 ejercicio6

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
	$(CC) $(CFLAGS) -o $@ $@.o -lpthread
	@echo "## ------------------------------"
	@echo "## Generado ejecutable $@ a partir de $^"
	@echo "## ------------------------------"
