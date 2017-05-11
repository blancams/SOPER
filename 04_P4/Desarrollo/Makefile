CC = gcc -std=gnu99
CFLAGS = -Wall
_EXE1 = cadena_montaje
_EXE2 = ejercicio_final
_EXE2AUX = apostador caballo monitor
_EXE2GES = gestor
_MODS = semaforos hilos senales memcomp mensajes
DIR1 = ejercicios/ejercicio1
DIR2 = ejercicios/ejercicio2
DIRR = recursos
EXE1 = $(patsubst %, $(DIR1)/%, $(_EXE1))
EXE2 = $(patsubst %, $(DIR2)/%, $(_EXE2))
EXE2AUX = $(patsubst %, $(DIR2)/%.o, $(_EXE2AUX))
EXE2GES = $(patsubst %, $(DIR2)/%.o, $(_EXE2GES))
MODS = $(patsubst %, $(DIRR)/%.o, $(_MODS))

all: $(EXE1) $(EXE2) $(EXE2AUX) $(EXE2GES) $(MODS)

.PHONY: clean
clean:
	rm -f $(DIR1)/*.o $(DIR2)/*.o $(EXE1) $(EXE2) $(DIRR)/*.o
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

$(EXE2): $(DIR2)/%: $(DIR2)/%.o $(MODS) $(EXE2AUX) $(EXE2GES)
	@echo "## ------------------------------"
	@echo "## Generado $@.o a partir de $@.c"
	@echo "## ------------------------------"
	$(CC) $(CFLAGS) -o $@ $@.o -lpthread $(MODS) $(EXE2AUX) $(EXE2GES)
	@echo "## ------------------------------"
	@echo "## Generado ejecutable $@ a partir de $^"
	@echo "## ------------------------------"

$(EXE2AUX): %.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo "## ------------------------------"
	@echo "## Generado $@.o a partir de $^"
	@echo "## ------------------------------"

$(EXE2GES): %.o: %.c %.h ejercicios/ejercicio2/apostador.h
	$(CC) $(CFLAGS) -c -o $@ $< -lpthread
	@echo "## ------------------------------"
	@echo "## Generado $@.o a partir de $^"
	@echo "## ------------------------------"

$(MODS): %.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo "## ------------------------------"
	@echo "## Generado $@.o a partir de $^"
	@echo "## ------------------------------"
