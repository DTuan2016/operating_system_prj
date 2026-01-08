PREFIX			= /usr/local
BIN_DIR			= $(PREFIX)/bin
INSTALL_PATH	= $(BIN_DIR)/$(TARGET)

CC 				= gcc
CFLAGS			= -Wall -Wextra -O2 -Iinclude
TARGET 			= saferun
SRC 			= main.c $(wildcard src/*.c)
TEST_SRC 		= test.c
TEST 			= test
SYS_TABLE 		= include/syscall_table.h

# Vô hiệu hóa mọi quy tắc ngầm định của Make
.SUFFIXES:

# all: $(SYS_TABLE) $(TARGET)
all: $(TARGET) $(TEST)

$(SYS_TABLE):
	@chmod +x ./configure.sh && ./configure.sh

$(TARGET): $(SRC)
	@echo "[INFO] Compiling directly to binary..."
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
	@echo "[INFO] Done! No .o files created."

$(TEST): $(TEST_SRC)
	@echo "[INFO] Compiling directly to binary..."
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST)
	@echo "[INFO] Done! No .o files created."	

clean:
	rm -f $(TARGET)
	rm -f $(TEST)

install:
	@mkdir -p $(BIN_DIR)
	@cp $(TARGET) $(INSTALL_PATH)
	@chmod +x $(INSTALL_PATH)
	@echo "[INFO] Installed → $(INSTALL_PATH)"
	@echo "Example usage:"
	@echo "  saferun -d:read,write -- ./app"

uninstall:
	rm -f $(INSTALL_PATH)
	@echo "[INFO] Uninstalled $(INSTALL_PATH)"

.PHONY: all clean install uninstall