# Makefile para o algoritmo de Prim
CXX = g++
CXXFLAGS = -Wall -O2

# Nome do executável
TARGET = prim

# Nome do arquivo fonte
SOURCE = prim.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)