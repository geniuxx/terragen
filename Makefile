# ============================================================================
# Makefile per Mountains 3D - Windows/MinGW
# ============================================================================

# Configurazione compiler
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O3 -march=native -ffast-math

# Evita la visualizzazione del terminale
CFLAGS_EXT = -Wl,--subsystem,windows  

LDFLAGS = -s

# Percorsi Raylib (modifica questi se necessario)
RAYLIB_PATH = C:/raylib
RAYLIB_INCLUDE = -I$(RAYLIB_PATH)/include
RAYLIB_LIB = -L$(RAYLIB_PATH)/lib

# Librerie necessarie per Windows
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lm

# Nome eseguibile
TARGET = terragen.exe

# File sorgente
SRC = terragen.c

# ============================================================================
# Targets
# ============================================================================

.PHONY: all clean run release debug help

# Target predefinito
all: $(TARGET)

# Compila il programma
$(TARGET): $(SRC)
	@echo Compilazione in corso...
	$(CC) $(CFLAGS) $(CFLAGS_EXT) $(RAYLIB_INCLUDE) $(SRC) -o $(TARGET) $(RAYLIB_LIB) $(LIBS) $(LDFLAGS)
	@echo Compilazione completata: $(TARGET)

# Versione release ottimizzata
release: CFLAGS += -DNDEBUG -flto
release: CFLAGS_EXT = -Wl,--subsystem,windows 
release: clean $(TARGET)
	@echo Build release completata

# Versione debug con simboli
debug: CFLAGS = -Wall -Wextra -std=c99 -g -O0
debug: CFLAGS_EXT =  
debug: LDFLAGS =
debug: clean $(TARGET)
	@echo Build debug completata

# Esegui il programma
run: $(TARGET)
	@echo Esecuzione $(TARGET)...
	./$(TARGET)

# Pulisci i file generati
clean:
	@echo Pulizia file...
	@if exist $(TARGET) del $(TARGET)
	@if exist *.o del *.o
	@echo Pulizia completata

# Mostra informazioni
help:
	@echo ========================================
	@echo Mountains 3D - Makefile per Windows
	@echo ========================================
	@echo.
	@echo Targets disponibili:
	@echo   make          - Compila il programma
	@echo   make release  - Compila versione ottimizzata
	@echo   make debug    - Compila versione debug
	@echo   make run      - Compila ed esegue
	@echo   make clean    - Rimuove file compilati
	@echo   make help     - Mostra questo messaggio
	@echo.
	@echo Configurazione:
	@echo   Compiler: $(CC)
	@echo   Flags:    $(CFLAGS)
	@echo   Raylib:   $(RAYLIB_PATH)
	@echo.

# Mostra la versione del compiler
info:
	@echo ========================================
	@echo Informazioni sistema
	@echo ========================================
	@$(CC) --version
	@echo.
	@echo RAYLIB_PATH = $(RAYLIB_PATH)
