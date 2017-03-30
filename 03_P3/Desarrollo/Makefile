CC = gcc -std=gnu99
CFLAGS = -Wall
EXE = 

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
