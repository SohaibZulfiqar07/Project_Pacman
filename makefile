# Variables for the compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system

# The name of your output file
TARGET = pacmanSfml

# The source file
SRC = pacmanSfml.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
	./$(TARGET)

clean:
	rm -f $(TARGET)