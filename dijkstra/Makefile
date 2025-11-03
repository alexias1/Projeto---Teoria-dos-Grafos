# Makefile para o algoritmo de Dijkstra

# Compilador C++
CXX = g++
# Flags de compilação: -Wall (mostra todos os avisos), -O2 (nível de otimização 2)
# Inclui c++11 para garantir compatibilidade com estruturas como auto, long long, etc.
CXXFLAGS = -Wall -O2 -std=c++11

# Nome do executável final (binário)
TARGET = dijkstra

# Nome do arquivo fonte
SOURCE = dijkstra.cpp

# Alvo padrão: compila o executável
all: $(TARGET)

# Regra para criar o executável:
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Alvo para limpar arquivos gerados (executável)
clean:
	rm -f $(TARGET)