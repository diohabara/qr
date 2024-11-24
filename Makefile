# Variables
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
SRCDIR = src
BUILDDIR = build
TARGET = qr

# Source and build files
SRC = $(SRCDIR)/qr.cc
OBJ = $(BUILDDIR)/qr.o

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
