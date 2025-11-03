#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <tuple>
#include <algorithm>
#include <sstream>

using namespace std;

// Define um tipo para aresta: {peso, vertice_destino, vertice_origem}
// Usaremos tuple para armazenar o vértice de origem, útil para a saída (-s)
using Aresta = tuple<int, int, int>; // {peso, destino, origem}

// Lista de adjacência: vector de pares {destino, peso}
using Grafo = vector<vector<pair<int, int>>>;

// Estrutura para a fila de prioridade: {peso, vertice_destino, vertice_origem}
// O std::priority_queue é um Max-Heap por padrão. Para Min-Heap, 
// armazenamos o peso negado ou usamos 'greater'. Usaremos 'greater'.
// {peso, vertice_atual} - 'vertice_origem' não é estritamente necessário aqui, 
// mas é útil para rastrear arestas.

// Para Prim, a priority queue armazena {peso, destino}
using PQ_Item = pair<int, int>; // {peso, destino}
// Min-Heap de PQ_Item
using MinHeap = priority_queue<PQ_Item, vector<PQ_Item>, greater<PQ_Item>>;

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

        // O grafo para AGM é não-direcionado, então adicionamos a aresta nas duas direções.
        adj[u].push_back({v, weight});
        adj[v].push_back({u, weight}); 
    }
    return true;
}

// Função para o Algoritmo de Prim
// Retorna o custo total da AGM
long long prim_algorithm(const Grafo& adj, int num_vertices, int start_node, vector<pair<int, int>>& mst_edges) {
    // 1. Verificação inicial e estruturas auxiliares
    if (start_node <= 0 || start_node > num_vertices) {
        // Sinaliza erro de nó inicial, mas o main já deve ter tratado isso
        return -1; 
    }

    // in_mst[u] = true se o vértice u já foi incluído na AGM.
    vector<bool> in_mst(num_vertices + 1, false); 
    
    // MinHeap: armazena as arestas candidatas {peso, destino}
    MinHeap pq; 
    
    long long total_cost = 0;
    
    // Contará o número de arestas incluídas na AGM. Deve ser V-1.
    int edges_in_mst_count = 0; 

    // min_weight[v] armazena o menor peso da aresta que conecta v à AGM atual.
    vector<int> min_weight(num_vertices + 1, 2e9); // 'infinito' grande o suficiente
    
    // parent_vertex[v] armazena o vértice u que trouxe v para a AGM pela aresta mínima.
    vector<int> parent_vertex(num_vertices + 1, -1);

    // 2. Inicialização do Algoritmo
    min_weight[start_node] = 0;
    // Adiciona o nó inicial com peso 0. Este item não representa uma aresta, mas inicia o processo.
    pq.push({0, start_node}); 
    
    // 3. Loop Principal (Executa V vezes, mas a primeira é a inicialização)
    // A condição de parada é que a PQ não esteja vazia E que V vértices sejam processados (ou V-1 arestas adicionadas).
    // Usaremos a contagem de arestas: edges_in_mst_count < num_vertices - 1.
    while (!pq.empty() && edges_in_mst_count < num_vertices - 1) { 
        
        // Pega o item do topo (aresta de menor peso para um vértice "fora")
        auto [weight, u] = pq.top();
        pq.pop();

        if (in_mst[u]) {
            continue; // Já na AGM, ignore
        }

        // 4. Inclusão do Vértice 'u' na AGM
        in_mst[u] = true;

        // Se o peso for > 0, significa que esta é uma aresta real que conecta o
        // novo vértice 'u' à AGM (e não a iteração inicial de peso 0).
        if (weight > 0) {
            total_cost += weight;
            edges_in_mst_count++; // Aresta real adicionada
            
            // Registra a aresta (u, parent_vertex[u])
            int p = parent_vertex[u];
            // Garante que a aresta seja registrada em ordem crescente (u, v)
            mst_edges.push_back({min(u, p), max(u, p)}); 
        }
        
        // 5. Exploração dos Vizinhos de 'u'
        for (const auto& edge : adj[u]) {
            int v = edge.first;
            int edge_weight = edge.second;

            // Se o vizinho 'v' não está na AGM E a aresta (u, v) é mais barata
            // do que a melhor aresta conhecida que conecta 'v' à AGM.
            if (!in_mst[v] && edge_weight < min_weight[v]) {
                
                // Atualiza o peso mínimo e o pai para 'v'
                min_weight[v] = edge_weight;
                parent_vertex[v] = u;
                
                // Adiciona/Atualiza a aresta candidata na Fila de Prioridade
                pq.push({edge_weight, v});
            }
        }
    }
    
    // 6. Verificação final (opcional, mas bom para robustez)
    // Se o número de arestas adicionadas for menor que V-1, o grafo não é conexo.
    //if (num_vertices > 0 && edges_in_mst_count != num_vertices - 1) {
        // Para este projeto, assumiremos conectividade ou retornaremos o custo da FGM (o que já fazemos).
        // Se o problema exigisse -1 para desconexo, a lógica seria adicionada aqui.
        // Como o custo é somado, o valor final é o custo da Floresta Geradora Mínima.
    //}

    return total_cost;
}

void print_help() {
    cout << "Uso: ./prim -f <arquivo> -i <vertice_inicial> [-s] [-o <saida>]" << endl;
    cout << "  -h : mostra este help" << endl;
    cout << "  -f : indica o arquivo que contem o grafo de entrada" << endl;
    cout << "  -i : vertice inicial (para o algoritmo de Prim)" << endl;
    cout << "  -s : mostra a solucao (arestas da AGM)" << endl;
    cout << "  -o : redireciona a saida para o arquivo" << endl;
}

int main(int argc, char* argv[]) {
    string filename = "";
    int start_node = -1;
    bool show_solution = false;
    string output_file = "";

    // Loop simples para processar argumentos
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
        } else if (arg == "-s") {
            show_solution = true;
        } else if (arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        }
    }

    // Checagem obrigatória
    if (filename.empty() || start_node == -1) {
        cerr << "Erro: Os parametros -f e -i sao obrigatorios." << endl;
        print_help();
        return 1;
    }
    
    // O restante do código de leitura e execução de Prim vai aqui...
    Grafo adj;
    int num_vertices;
    
    // 1. Leitura
    if (!read_graph(filename, adj, num_vertices)) {
        return 1;
    }

    vector<pair<int, int>> mst_edges;
    
    // 2. Execução
    long long cost = prim_algorithm(adj, num_vertices, start_node, mst_edges);

    if (cost == -1) {
        cerr << "Erro na execucao do algoritmo de Prim." << endl;
        return 1;
    }

    // 3. Saída (com ou sem -o)
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
    
    // Output do custo
    *out << cost << endl;

    // Output da solução (-s)
    if (show_solution) {
        // Ordena as arestas para uma saída mais consistente, embora o projeto diga que não é obrigatório
        // sort(mst_edges.begin(), mst_edges.end());
        
        for (const auto& edge : mst_edges) {
            *out << "(" << edge.first << "," << edge.second << ") ";
        }
        *out << endl;
    }

    return 0;
}