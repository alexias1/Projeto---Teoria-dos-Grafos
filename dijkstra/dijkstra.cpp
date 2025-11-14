#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <sstream>
#include <cstdio>

using namespace std;

// Lista de Adjacência: vector de pares {destino, peso}
using Grafo = vector<vector<pair<int, int>>>; // {destino, peso}

// Estrutura para a Fila de Prioridade: {distancia_total, vertice}
using PQ_Item = pair<long long, int>; // {distancia_total, vertice}
using MinHeap = priority_queue<PQ_Item, vector<PQ_Item>, greater<PQ_Item>>;

// Constante para representar o infinito (long long para distâncias grandes)
// Escolhendo um valor seguro para o infinito.
const long long INFINITO = numeric_limits<long long>::max(); 


// Função principal do Algoritmo de Dijkstra
// Retorna um vetor de distâncias do start_node para todos os outros vértices
vector<long long> dijkstra_algorithm(const Grafo& adj, int num_vertices, int start_node) {
    
    // Vetor de distâncias: distancias[v] armazena a menor distância de start_node até v.
    vector<long long> distancias(num_vertices + 1, INFINITO);
    
    // Fila de Prioridade: {distancia_total, vertice}
    MinHeap pq;
    
    // 1. Inicialização
    distancias[start_node] = 0;
    // Adiciona o nó inicial à PQ.
    pq.push({0, start_node}); 
    
    // 2. Loop de Relaxamento
    while (!pq.empty()) {
        
        // Seleção: Pega o vértice u com a menor distância total
        long long d_atual = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        // Verificação de Obsoleto: Se a distância retirada da PQ for maior que 
        // a distância conhecida (já relaxada), ignora o item.
        if (d_atual > distancias[u]) {
            continue;
        }

        // Exploração dos Vizinhos (Relaxamento)
        for (const auto& aresta : adj[u]) {
            int v = aresta.first;
            int peso_aresta = aresta.second;
            
            // Calcula a distância através de u
            long long d_nova = d_atual + peso_aresta;
            
            // Relaxamento: Se a nova distância for menor que a distância atual de v, atualiza!
            if (d_nova < distancias[v]) {
                
                // Atualiza a distância
                distancias[v] = d_nova;
                
                // Insere a nova distância na PQ para que v seja processado
                pq.push({d_nova, v});
            }
        }
    }
    
    // 3. Pós-processamento: Trata o INFINITO para o valor exigido (-1)
    for (int i = 1; i <= num_vertices; ++i) {
        if (distancias[i] == INFINITO) {
            distancias[i] = -1; // Vértice inalcançável
        }
    }
    return distancias;
}

void print_help() {
    cout << "Uso: ./dijkstra -f <arquivo> -i <vertice_inicial> [-o <saida>]" << endl;
    cout << "  -h : mostra este help" << endl;
    cout << "  -o : redireciona a saida para o arquivo" << endl;
    cout << "  -f : indica o arquivo que contem o grafo de entrada" << endl;
    cout << "  -i : vertice inicial" << endl;
}

bool read_graph(const string& filename, Grafo& adj, int& num_vertices) {
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

    // Inicializa a lista de adjacência (V+1 para grafos baseados em 1)
    adj.assign(num_vertices + 1, vector<pair<int, int>>());

    int u, v, weight;
    for (int i = 0; i < num_edges; ++i) {
        if (!(file >> u >> v >> weight)) {
            cerr << "Erro ao ler aresta " << i + 1 << "." << endl;
            return false;
        }
        
        // Assumimos que o grafo é direcionado ou que a duplicação é feita na entrada.
        // Pelo formato do Prim, duplicamos para não-direcionado, o que é seguro para Dijkstra.
        // Se o problema exige estritamente direcionado, remova a linha de v -> u.
        adj[u].push_back({v, weight});
        adj[v].push_back({u, weight}); // Mantido como não-direcionado, como nos exemplos AGM
    }

    return true;
}

int main(int argc, char* argv[]) {
    string filename = "";
    int start_node = -1; // Padrão é -1 (inválido)
    string output_file = "";
    bool show_solution_ignored = false; 

    // 1. Parsing de Argumentos
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-h") {
            print_help();
            return 0;
        } else if (arg == "-f" && i + 1 < argc) {
            filename = argv[++i];
        } else if (arg == "-i" && i + 1 < argc) {
            try {
                start_node = stoi(argv[++i]);
            } catch (...) {
                cerr << "Erro: Vertice inicial (-i) invalido." << endl;
                return 1;
            }
        } else if (arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        } else if (arg == "-s") {
            show_solution_ignored = true; // Ignora o -s
        }
    }

    // *** CORREÇÃO: Lidar com -i ausente (Script Original) ***
    if (filename.empty()) {
        print_help();
        return 1;
    }
    // Se o -i não foi fornecido (ainda é -1), assume 1 como padrão.
    if (start_node == -1) {
        start_node = 1; 
    }
    
    Grafo adj;
    int num_vertices;
    
    if (!read_graph(filename, adj, num_vertices)) {
        return 1;
    }

    vector<long long> distancias = dijkstra_algorithm(adj, num_vertices, start_node);

    // Configuração da Saída (stdout ou arquivo)
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
    
    // *** CORREÇÃO: Formatação de Saída (printf) ***
    if (out == &cout) {
        // Se for stdout (o script lê daqui), use printf
        for (int i = 1; i <= num_vertices; ++i) {
            printf("%d:%lld ", i, distancias[i]);
        }
        printf("\n");
    } else {
        // Se for para arquivo (-o), C++ streams são seguros
        for (int i = 1; i <= num_vertices; ++i) {
            *out << i << ":" << distancias[i] << " ";
        }
        *out << '\n';
    }

    return 0;
}