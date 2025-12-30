CC=gcc
CFLAGS=-Wall -Wextra -O2 -Iinclude
LDFLAGS=

TARGET=saferun

SRC=main.c $(wildcard src/*.c)
OBJ=$(SRC:.c=.o)

SYS_TABLE=include/syscall_table.h

PREFIX=/usr/local
BIN_DIR=$(PREFIX)/bin
INSTALL_PATH=$(BIN_DIR)/$(TARGET)

# ------------------------------------------

all: $(SYS_TABLE) $(TARGET)

# Generate syscall table if missing
$(SYS_TABLE):
	@echo "[*] Generating syscall_table.h ..."
	@chmod +x ./configure.sh
	@./configure.sh

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[✔] Build completed -> ./$(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o src/*.o $(TARGET)
	@echo "[*] Clean done."

install: all
	@mkdir -p $(BIN_DIR)
	@cp $(TARGET) $(INSTALL_PATH)
	@chmod +x $(INSTALL_PATH)
	@echo "[✔] Installed → $(INSTALL_PATH)"
	@echo "Example usage:"
	@echo "  saferun --allow read,write -- ./app"

uninstall:
	rm -f $(INSTALL_PATH)
	@echo "[✔] Uninstalled $(INSTALL_PATH)"

.PHONY: all clean install uninstall
