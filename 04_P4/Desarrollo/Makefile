CC = gcc -std=gnu99
CFLAGS = -Wall
_EXE1 = cadena_montaje
_EXE2 = ejercicio_final
DIR1 = ejercicios/ejercicio1
DIR2 = ejercicios/ejercicio2
EXE1 = $(patsubst %, $(DIR1)/%, $(_EXE1))
EXE2 = $(patsubst %, $(DIR2)/%, $(_EXE2))
_MODS = semaforos/semaforos hilos/hilos senales/senales memcomp/memcomp mensajes/mensajes
MODS = $(patsubst %, %.o, $(_MODS))

all: $(EXE1) $(EXE2) $(MODS)

.PHONY: clean
clean:
	rm -f $(DIR1)/*.o $(DIR2)/*.o $(EXE1) $(EXE2) $(MODS)
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

$(EXE2): $(DIR2)/%: $(DIR2)/%.o $(MODS)
	@echo "## ------------------------------"
	@echo "## Generado $@.o a partir de $@.c"
	@echo "## ------------------------------"
	$(CC) $(CFLAGS) -o $@ $@.o -lpthread $(MODS)
	@echo "## ------------------------------"
	@echo "## Generado ejecutable $@ a partir de $^"
	@echo "## ------------------------------"
