.PHONY: all clean

BUILD_DIR := build
$(shell mkdir -p $(BUILD_DIR))

SRC := main.cpp Test.cpp
OBJ := $(BUILD_DIR)/main.o $(BUILD_DIR)/Test.o
HEADER := Test.h

all: $(BUILD_DIR)/executable

$(BUILD_DIR)/%.o : %.cpp $(HEADER)
	$(CXX) $(CPPFLAGS) -I./ -c $< -o $@

$(BUILD_DIR)/executable: $(OBJ)
	$(CXX) $(CPPFLAGS) $^ -o $@

clean:
	rm -rf build/
