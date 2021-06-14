.PHONY: all clean

BUILD_DIR := build
$(shell mkdir -p $(BUILD_DIR))

SRC := main.c Test.c
OBJ := $(BUILD_DIR)/main.o $(BUILD_DIR)/Test.o
HEADER := Test.h

all: $(BUILD_DIR)/executable
	@echo "Finished!"

$(BUILD_DIR)/%.o : %.c $(HEADER)
	$(CC) $(CFLAGS) -I./ -c $< -o $@

$(BUILD_DIR)/executable: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf build/
