#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <tuple>
#include <cstdio>

using namespace std;

// Implementação da estrutura Union-Find (Conjuntos Disjuntos)
class UnionFind {
private:
    vector<int> pai; // Vetor que armazena o pai de cada elemento
    // Opcional: vector<int> rank ou size para otimização por rank/tamanho

public:
    // Construtor: Inicializa N conjuntos disjuntos (Vértices 1 a N)
    UnionFind(int N) {
        // Vértices são 1-based, então N+1
        pai.assign(N + 1, 0); 
        for (int i = 1; i <= N; ++i) {
            pai[i] = i; // Inicialmente, cada elemento é seu próprio pai/representante
        }
    }

    // Função Find: Retorna o representante do conjunto de 'i'
    // Usa compressão de caminho para eficiência
    int Find(int i) {
        if (pai[i] == i)
            return i;
        // Compressão de caminho: faz o nó 'i' apontar diretamente para a raiz
        return pai[i] = Find(pai[i]); 
    }

    // Função Union: Une os conjuntos que contêm 'i' e 'j'
    // Retorna true se a união ocorreu (sem ciclo), false se já estavam unidos (ciclo)
    bool Union(int i, int j) {
        int raiz_i = Find(i);
        int raiz_j = Find(j);

        if (raiz_i != raiz_j) {
            // Une as duas árvores (arbitrariamente: raiz_j aponta para raiz_i)
            pai[raiz_j] = raiz_i; 
            return true; // União bem-sucedida (sem ciclo)
        }
        return false; // Raízes iguais: já estão no mesmo conjunto (ciclo detectado)
    }
};

// Define uma aresta para o Kruskal: {peso, vertice_u, vertice_v}
// O peso é o primeiro elemento para que o std::sort funcione corretamente
using ArestaKruskal = tuple<int, int, int>; // {peso, u, v}
using ListaArestas = vector<ArestaKruskal>;

bool read_edges(const string& filename, ListaArestas& arestas, int& num_vertices) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo: " << filename << endl;
        return false;
    }

    int num_edges;
    if (!(file >> num_vertices >> num_edges)) {
        cerr << "Erro ao ler V e M do arquivo." << endl;
        return false;
    }

    int u, v, weight;
    for (int i = 0; i < num_edges; ++i) {
        if (!(file >> u >> v >> weight)) {
            cerr << "Erro ao ler aresta " << i + 1 << "." << endl;
            return false;
        }

        // Checagem de validade do vértice
        if (u < 1 || u > num_vertices || v < 1 || v > num_vertices) {
            cerr << "Erro: Vertice " << u << " ou " << v << " fora do intervalo [1, " << num_vertices << "]." << endl;
            return false;
        }

        // Adiciona a aresta à lista: {peso, u, v}
        arestas.emplace_back(weight, u, v); 
    }

    return true;
}

// Função para o Algoritmo de Kruskal
// Retorna o custo total da AGM
long long kruskal_algorithm(ListaArestas& arestas, int num_vertices, vector<pair<int, int>>& mst_edges) {
    
    // 1. Ordenação: Crucial para a lógica gulosa
    // std::sort em um tuple ordena pelo primeiro elemento (peso).
    sort(arestas.begin(), arestas.end()); 

    // 2. Inicialização do Union-Find
    UnionFind uf(num_vertices);
    
    long long total_cost = 0;
    int edges_in_mst_count = 0;

    // 3. Loop Guloso
    // Itera sobre as arestas ordenadas
    for (const auto& aresta : arestas) {
        int weight = get<0>(aresta);
        int u = get<1>(aresta);
        int v = get<2>(aresta);

        // Teste de Ciclo: Verifica se u e v JÁ estão no mesmo componente
        if (uf.Find(u) != uf.Find(v)) {
            
            // Não forma ciclo: Adiciona a aresta à AGM
            uf.Union(u, v); // Une os conjuntos
            
            total_cost += weight;
            edges_in_mst_count++;

            // Registra a aresta para a saída (-s)
            mst_edges.push_back({min(u, v), max(u, v)});

            // Condição de parada: AGM completa
            if (edges_in_mst_count == num_vertices - 1) {
                break; 
            }
        }
    }
    
    // O Kruskal naturalmente retorna a FGM se o grafo for desconexo.
    // O custo total será a soma das arestas encontradas.

    return total_cost;
}

void print_help() {
    cout << "Uso: ./kruskal -f <arquivo> [-s] [-o <saida>]" << endl;
    cout << "  -h : mostra este help" << endl;
    cout << "  -o : redireciona a saida para o arquivo" << endl;
    cout << "  -f : indica o arquivo que contem o grafo de entrada" << endl;
    cout << "  -s : mostra a solucao" << endl;
    cout << "  -i : vertice inicial" << endl;
}   

int main(int argc, char* argv[]) {
    // Variáveis de controle de parâmetros
    string filename = "";
    bool show_solution = false;
    string output_file = "";
    
    // O Kruskal não precisa de start_node, mas o projeto permite o parâmetro
    int start_node_ignored = -1; 

    // Loop para processar argumentos
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-h") {
            print_help();
            return 0;
        } else if (arg == "-f" && i + 1 < argc) {
            filename = argv[++i];
        } else if (arg == "-s") {
            show_solution = true;
        } else if (arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        } else if (arg == "-i" && i + 1 < argc) {
             // Aceita o parâmetro -i, mas o ignora para o Kruskal
            try {
                start_node_ignored = stoi(argv[++i]);
            } catch (...) {
                // Não faz nada, o Kruskal não usa
            }
        }
    }

    // Checagem obrigatória
    if (filename.empty() || start_node_ignored == -1) {
        if (start_node_ignored == -1) {
        start_node_ignored = 1; // Ponto de partida padrão
        }
        if (filename.empty()) {
            print_help();
            return 1;
        }
    }
    
    ListaArestas arestas; // Lista que armazenará todas as arestas
    int num_vertices;
    
    // 1. Leitura do Grafo (Arestas)
    if (!read_edges(filename, arestas, num_vertices)) {
        return 1;
    }

    vector<pair<int, int>> mst_edges; // Vetor para armazenar as arestas da AGM
    
    // 2. Execução do Algoritmo de Kruskal
    long long cost = kruskal_algorithm(arestas, num_vertices, mst_edges);

    // 3. Configuração da Saída (stdout ou arquivo)
    ostream* out = &cout;
    ofstream outfile;
    if (!output_file.empty()) {
        outfile.open(output_file);
        if (outfile.is_open()) {
            out = &outfile;
        } else {
            cerr << "Erro: Nao foi possivel abrir o arquivo de saida: " << output_file << endl;
            // Se o arquivo não abrir, imprime no stdout
        }
    }
    
    // Output do custo OU da solução, mas NUNCA ambos.
    if (show_solution) {
        // Output da solução (-s)
        for (const auto& edge : mst_edges) {
            *out << "(" << edge.first << "," << edge.second << ") ";
        }
        std::putc('\n', stdout);
    } else {
        // Output do custo (apenas se -s NÃO foi solicitado)
        std::putc('\n', stdout);
    }

    return 0;
}