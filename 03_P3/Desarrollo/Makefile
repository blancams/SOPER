CC = gcc -std=gnu99
CFLAGS = -Wall
_EXE1 = ejercicio2 ejercicio3
_EXE2 = ejercicio5
_LIB = semaforos
DIR1 = ej_2_3
DIR2 = ej_5_6
EXE1 = $(patsubst %, $(DIR1)/%, $(_EXE1))
EXE2 = $(patsubst %, $(DIR2)/%, $(_EXE2))
LIB = $(patsubst %, %.o, $(_LIB))

all: $(EXE1) $(EXE2) semaforos.o

.PHONY: clean
clean:
	rm -f $(DIR1)/*.o $(DIR2)/*.o $(EXE1) $(EXE2) $(LIB)
	@echo "## ------------------------------"
	@echo "## Archivos objeto y ejecutables eliminados."
	@echo "## ------------------------------"

$(EXE1): $(DIR1)/%: $(DIR1)/%.o
	@echo "## ------------------------------"
	@echo "## Generado $@.o a partir de $@.c"
	@echo "## ------------------------------"
	$(CC) $(CFLAGS) -o $@ $@.o
	@echo "## ------------------------------"
	@echo "## Generado ejecutable $@ a partir de $^"
	@echo "## ------------------------------"

$(EXE2): $(DIR2)/%: $(DIR2)/%.o $(LIB)
	@echo "## ------------------------------"
	@echo "## Generado $@.o a partir de $@.c"
	@echo "## ------------------------------"
	$(CC) $(CFLAGS) -o $@ $@.o $(LIB)
	@echo "## ------------------------------"
	@echo "## Generado ejecutable $@ a partir de $^"
	@echo "## ------------------------------"
