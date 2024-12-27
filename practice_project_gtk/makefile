# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = $(shell pkg-config --cflags gtk+-3.0)

# Linker Flags
LDFLAGS = $(shell pkg-config --libs gtk+-3.0) -lpthread

# Target and Source Files
TARGET = app_gtk
SRC = app_gtk.cpp


# Default Rule
all: $(TARGET)

# Build the target
$(TARGET): $(SRC) 
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(LDFLAGS)

# Clean rule
clean:
	rm -f $(TARGET)

.PHONY: all clean

