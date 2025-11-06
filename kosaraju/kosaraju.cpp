#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stack>
#include <string>

using namespace std;

// Lista de Adjacência para Grafo Direcionado (sem pesos)
using Grafo = vector<vector<int>>; 

// Estruturas de Suporte (Ajudam a organizar o código principal)

void print_help() {
    cout << "Uso: ./kosaraju -f <arquivo> [-o <saida>]" << endl;
    cout << "  -h : mostra este help" << endl;
    cout << "  -o : redireciona a saida para o arquivo" << endl;
    cout << "  -f : indica o arquivo que contem o grafo de entrada" << endl;
}

// Função de Leitura (para Grafo Direcionado, sem pesos)
// Lê G (Grafo Original) e constrói GT (Grafo Transposto) simultaneamente
bool read_graph(const string& filename, Grafo& G, Grafo& GT, int& num_vertices) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo: " << filename << endl;
        return false;
    }

    int num_edges;
    // Tenta ler o número de vértices e arestas
    if (!(file >> num_vertices >> num_edges)) {
        cerr << "Erro ao ler V e M do arquivo." << endl;
        return false;
    }

    // Inicializa G (Grafo Original) e GT (Grafo Transposto)
    // Dimensionado para num_vertices + 1 para lidar com vértices 1-based
    G.assign(num_vertices + 1, vector<int>());
    GT.assign(num_vertices + 1, vector<int>());

    int u, v;
    for (int i = 0; i < num_edges; ++i) {
        if (!(file >> u >> v)) {
            cerr << "Erro ao ler aresta " << i + 1 << "." << endl;
            return false;
        }
        
        // Grafo Original: Aresta u -> v
        G[u].push_back(v);
        
        // Grafo Transposto: Aresta v -> u (Direção invertida)
        GT[v].push_back(u); 
    }

    return true;
}


// DFS para o Passo 1: Preenche a pilha com a ordem de finalização
void DFS_Pass1(const Grafo& G, int u, vector<bool>& visitados, stack<int>& pilha_finalizacao) {
    visitados[u] = true;
    
    // 1. Explorar Vizinhos
    for (int v : G[u]) {
        if (!visitados[v]) {
            DFS_Pass1(G, v, visitados, pilha_finalizacao);
        }
    }

    // 2. Ação: Adiciona o vértice à pilha APÓS visitar todos os seus descendentes.
    pilha_finalizacao.push(u); 
}

// DFS para o Passo 2: Encontra a CFC no Grafo Transposto (GT)
void DFS_Pass2(const Grafo& GT, int u, vector<bool>& visitados, vector<int>& componente_atual) {
    visitados[u] = true;
    // 1. Ação: Adiciona o vértice à CFC que está sendo construída.
    componente_atual.push_back(u); 
    
    // 2. Explorar Vizinhos (no grafo Transposto!)
    for (int v : GT[u]) {
        if (!visitados[v]) {
            DFS_Pass2(GT, v, visitados, componente_atual);
        }
    }
}


// Função principal do Algoritmo de Kosaraju
// Retorna um vector<vector<int>> onde cada vector interno é uma CFC
vector<vector<int>> kosaraju_algorithm(const Grafo& G, const Grafo& GT, int num_vertices) {
    
    // 1. Passo 1: DFS no Grafo Original (G) para obter a ordem de finalização
    
    vector<bool> visitados(num_vertices + 1, false);
    stack<int> pilha_finalizacao; 

    // Itera sobre todos os vértices (para lidar com grafos desconexos)
    for (int i = 1; i <= num_vertices; ++i) {
        if (!visitados[i]) {
            DFS_Pass1(G, i, visitados, pilha_finalizacao); 
        }
    }
    
    // 2. Passo 2: DFS no Grafo Transposto (GT) na Ordem Inversa de Finalização
    
    vector<vector<int>> cfcs; // Resultado final
    fill(visitados.begin(), visitados.end(), false); // Reseta o vetor de visitados
    
    // Processa os vértices na ordem inversa (do topo da pilha para a base)
    while (!pilha_finalizacao.empty()) {
        int u = pilha_finalizacao.top();
        pilha_finalizacao.pop();

        // Se o vértice ainda não foi visitado, ele é o "líder" de uma nova CFC
        if (!visitados[u]) {
            vector<int> componente_atual; // Vetor para a nova CFC
            
            // Executa DFS no GT a partir do líder para encontrar todos os membros da CFC
            DFS_Pass2(GT, u, visitados, componente_atual);
            
            cfcs.push_back(componente_atual); 
        }
    }
    
    return cfcs;
}


int main(int argc, char* argv[]) {
    string filename = "";
    string output_file = "";

    // 1. Parsing de Argumentos
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-h") {
            print_help();
            return 0;
        } else if (arg == "-f" && i + 1 < argc) {
            filename = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        }
    }

    if (filename.empty()) {
        cerr << "Erro: O parametro -f e obrigatorio." << endl;
        print_help();
        return 1;
    }
    
    Grafo G, GT;
    int num_vertices;
    
    // 2. Leitura do Grafo
    if (!read_graph(filename, G, GT, num_vertices)) {
        return 1;
    }

    // 3. Execução
    vector<vector<int>> cfcs = kosaraju_algorithm(G, GT, num_vertices);
    
    // 4. Configuração da Saída
    ostream* out = &cout;
    ofstream outfile;
    if (!output_file.empty()) {
        outfile.open(output_file);
        if (outfile.is_open()) {
            out = &outfile;
        } else {
            cerr << "Erro: Nao foi possivel abrir o arquivo de saida: " << output_file << endl;
        }
    }
    
    // 5. Output das Componentes Fortemente Conexas (CFCs)
    for (const auto& cfc : cfcs) {
        for (int vertice : cfc) {
            *out << vertice << " ";
        }
        *out << endl;
    }

    return 0;
}