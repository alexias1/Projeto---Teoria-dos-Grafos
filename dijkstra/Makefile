CXX = g++

CXXFLAGS = -Wall -O2 -std=c++11

TARGET = dijkstra

SOURCE = dijkstra.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)