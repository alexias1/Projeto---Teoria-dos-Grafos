# Makefile para o algoritmo de Kruskal

# Compilador C++
CXX = g++
# Flags de compilação: -Wall (mostra todos os avisos), -O2 (nível de otimização 2)
CXXFLAGS = -Wall -O2

# Nome do executável final (binário)
TARGET = kruskal

# Nome do arquivo fonte
SOURCE = kruskal.cpp

# Alvo padrão: compila o executável
all: $(TARGET)

# Regra para criar o executável:
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Alvo para limpar arquivos gerados (executável)
clean:
	rm -f $(TARGET)