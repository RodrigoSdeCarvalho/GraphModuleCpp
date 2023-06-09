#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <unordered_map>
#include <tuple>
#include <algorithm>
#include <memory>

#include "Graph/UndirectedGraph.h"
#include "Graph/Node.h"
#include "Graph/Connection.h"
#include "Graph/MinHeap.h"
#include "Graph/HeapNode.h"

using namespace std;
using namespace GraphModule;


UndirectedGraph::UndirectedGraph()
{
    this->numberOfEdges = 0;
}

int UndirectedGraph::getDegreeOfNode(int nodeKey)
{
    shared_ptr<Node> node = this->nodes[nodeKey];
    vector<shared_ptr<Connection>> connections = node->getConnections();

    int degree = connections.size();

    
    for (const auto& nodeToCheck : this->nodes)
    {
        vector<shared_ptr<Connection>> connectionsToCheck = nodeToCheck->getConnections();

        for (const auto& connectionToCheck : connectionsToCheck)
        {
            shared_ptr<Node> nodeConnectedTo = connectionToCheck->getEndNode();

            if (nodeConnectedTo->getNumber() == node->getNumber())
            {
                degree++;
            }
        }
    }

    return degree;
}

void UndirectedGraph::addEdge(const shared_ptr<Node>& node1, const shared_ptr<Node>& node2, float weight)
{
    int n1 = node1->getNumber();
    int n2 = node2->getNumber();
    
    // find to check if the value is present in the vector
    auto findDomain = find(domain.begin(), domain.end(), n1);
    auto findContraDomain = find(contradomain.begin(), contradomain.end(), n2);

    if (findDomain == domain.end()) {
        domain.push_back(n1);
    }

    if (findContraDomain == contradomain.end()) {
        contradomain.push_back(n2);
    }

    weak_ptr<Node> node1WeakPtr(node1);
    weak_ptr<Node> node2WeakPtr(node2);

    shared_ptr<Connection> connection1SharedPtr = make_shared<Connection>(weight, node1WeakPtr, node2WeakPtr, true);
    shared_ptr<Connection> connection2SharedPtr = make_shared<Connection>(weight, node2WeakPtr, node1WeakPtr, true);
    
    this->edges.push_back(connection1SharedPtr);
    this->edges.push_back(connection2SharedPtr);

    weak_ptr<Connection> connection1WeakPtr(connection1SharedPtr);
    node1->addConnection(connection1SharedPtr);

    weak_ptr<Connection> connection2WeakPtr(connection2SharedPtr);
    node2->addConnection(connection2SharedPtr);

    this->numberOfEdges++;
}

tuple<vector<int>, vector<int>> UndirectedGraph::BFS(int startNodeIndex)
{
    int numberOfVertices = this->numberOfVertices; // Number of vertices in the graph.
    vector<shared_ptr<Node>> nodes = this->nodes;
    vector<bool> V; // Vector of visited nodes.
    vector<int> D; // Vector of distances from the start node.
    vector<int> A; // Vector of parent nodes.
    vector<vector<int>> levels; // Vector of vectors to store vertices found at each level
    for (int i = 0; i <= numberOfVertices; i++)
    {
        V.push_back(false); // Initialize with false.
        D.push_back(1000000); // Initialize with infinity.
        A.push_back(-1); // Initialize with -1.
    }
    V[startNodeIndex] = true; // Mark the start node as visited.
    D[startNodeIndex] = 0; // Set the distance from the start node to itself to 0.
    A[startNodeIndex] = -1; // Set the parent of the start node to -1.

    queue<int> Q; // Create a queue.
    Q.push(startNodeIndex); // Push the start node to the queue.
    levels.push_back(vector<int>({startNodeIndex})); // Add the start node to level 0
    while (!Q.empty())
    {
        int u = Q.front(); // Get the first element in the queue.
        Q.pop(); // Remove the first element in the queue.
        vector<shared_ptr<Connection>> connections = nodes[u]->getConnections(); // Get the connections of the start node.

        for (const auto& conn : connections)
        {
            shared_ptr<Node> vNode = (conn)->getEndNode(); // Get the end node of the connection.
            int v = vNode->getNumber() - 1; // Get the index of the end node of the connection.
            if (!V[v])
            { // If the end node of the connection is not visited.
                V[v] = true; // Mark the end node as visited.
                D[v] = D[u] + 1; // Set the distance from the start node to the end node to the distance from the start node to the start node of the connection plus 1.
                A[v] = u; // Set the parent of the end node to the start node of the connection.
                Q.push(v); // Push the end node to the queue.
            }
        }
    }

    return make_tuple(D, A);
}

void UndirectedGraph::printBFS(vector<int> D)
{
    // Print the levels and vertices found in each level.
    for (int level = 0; level <= numberOfVertices; level++) {
        bool foundVertices = false;
        for (int i = 0; i < numberOfVertices; i++) {
            if (D[i] == level) {
                if (!foundVertices) {
                    cout << level << ":";
                    foundVertices = true;
                }
                cout << " " << nodes[i]->getNumber();
            }
        }
        if (!foundVertices) {
            break; // No more vertices to print.
        }
        cout << endl;
    }
}

vector<int> UndirectedGraph::eulerianCycle(int startNodeIndex)
{
    int numberOfVertices = this->numberOfVertices; // Number of vertices in the graph.
    vector<vector<int>> C;
    int numberOfConnectionsBeginNode = this->getDegreeOfNode(startNodeIndex)/2;
    int beginNodeIndex = startNodeIndex;
    if (numberOfConnectionsBeginNode == 0) // If the initial node is not connected we don't have a cycle, it needs to be on the same connected component
    { 
        return vector<int>({0});
    }

    for (int i = 0; i < numberOfVertices; i++){
        if(this->nodes[i]->getConnections().size()%2 != 0) //If there is a node that has an odd degree we don't have a cycle
        {
            return vector<int>({0});
        }
    }

    for (int i = 0; i < numberOfVertices; i++) // Initialize with false the connections, if there isn't, initialize with infinity.
    {
        vector<int> connections;
        for (int j = 0; j < numberOfVertices; j++)
        {
            tuple<bool, shared_ptr<Connection>> returnedValues = this->nodes[i]->getConnectionWith(this->nodes[j]);
            if (get<0>(returnedValues))
            {
                connections.push_back(false); 
            }
            else 
            {
                connections.push_back(1000000);
            }
        }
        C.push_back(connections); 
    }

    tuple<bool, vector<int>, vector<vector<int>>> returnedValues = searchEulerianSubcycle(beginNodeIndex, C); // Searches for subcycles

    bool r = get<0>(returnedValues);
    vector<int> cycle = get<1>(returnedValues);
    vector<vector<int>> new_C = get<2>(returnedValues);
    
    if(!r) // There is no cycle
    { 
        return vector<int>({0});
    }
    else
    {
        bool allVisited = true; 
        for (const auto& line : new_C) // Searches for a false value in C, if there is at least one, all visited is false.
        {
  	        for (auto element : line)
            {
                allVisited = allVisited && element;
            }
        }
        if(!allVisited)
        { // There is a node that wasn't visited, so there is no cycle since there is one that is unconnected
            return vector<int>({0});
        }
        else
        {
            cout << "1" << endl;
            return cycle;
        }
    }
}

tuple<bool, vector<int>, vector<vector<int>>> UndirectedGraph::searchEulerianSubcycle(int beginNodeIndex, vector<vector<int>> C)
{
    vector<int> cycle;
    int v = beginNodeIndex;
    vector<vector<int>> adjMatrix = std::move(C);
    cycle.push_back(v);
    int t = v;

    do
    {
        int u = -1;
        auto neighbours = this->nodes[v]->getNeighbours();
        for (const auto& neighbour : neighbours)
        {
            int neighbourIndex = neighbour->getNumber() - 1;
            if (!adjMatrix[neighbourIndex][v]) { // Selects an edge that C=false
                u = neighbourIndex;
                break;
            }
        }
        if (u == -1) { // If u = -1 it means that there are no C=false
            return make_tuple(false, vector<int>({0}), adjMatrix);
        } else {
            adjMatrix[u][v] = true;
            adjMatrix[v][u] = true;
            v = u;
            cycle.push_back(v); // Adds v to the end of the cycle.
        }
    }while (t != v);

    for(auto i = cycle.begin(); i != cycle.end(); i++) // Checks for subcycles
    {
        int node = *i;
        auto neighbours = this->nodes[node]->getNeighbours();
        
        for (const auto& neighbour : neighbours)
        {
            int neighbourIndex = neighbour->getNumber() - 1;
            if (!adjMatrix[neighbourIndex][node]) 
            {
                tuple<bool, vector<int>, vector<vector<int>>> returnedValues = searchEulerianSubcycle(node, adjMatrix);
                adjMatrix = get<2>(returnedValues);
                bool r = get<0>(returnedValues);
                vector<int> subcycle = get<1>(returnedValues);

                if (r) 
                {
                    for(auto j = ++(subcycle.begin()); j != subcycle.end(); j++)
                    {
                        i = cycle.insert(i, *j);
                    }
                }
            }
        }
    }

    return make_tuple(true, cycle, adjMatrix);
}

void UndirectedGraph::printEulerianCycle(vector<int> cycle)
{

    for(long unsigned int i=0; i < cycle.size(); i++)
    {
        if (cycle.size()==1)
        {
            cout << cycle[i] << " ";
        }
        else
        {  
            cout << cycle[i] + 1 << " "; //+1 to print accordingly to the input
        }
    }
    cout << endl;
}

tuple<vector<int>, vector<int>> UndirectedGraph::dijkstra(int startNodeIndex)
{
    int numberOfVertices = this->numberOfVertices; // Number of vertices in the graph.
    vector<shared_ptr<Node>> nodes = this->nodes; // Vector of nodes in the graph.
    vector<int> D = vector<int>(numberOfVertices); // Vector of distances from the start node.
    vector<int> A = vector<int>(numberOfVertices); // Vector of parent nodes.
    vector<bool> C(numberOfVertices, false); // Vector of visited nodes.
    int visitedNodes = 0; // Counter for the number of visited nodes.

    for (int i = 0; i < numberOfVertices; i++)
    {
        D[i] = 1000000; // Initialize with infinity.
        A[i] = -1; // Initialize with -1.
    }

    D[startNodeIndex] = 0; // Set the distance from the start node to itself to 0.
    
    A[startNodeIndex] = -1; // Set the parent of the start node to -1.

    MinHeap minHeap = MinHeap(); // Create a min heap. 
    for (int i = 0; i < numberOfVertices; i++)
    {
        minHeap.insert(this->nodes[i], D[i]);
    }

    while (visitedNodes < numberOfVertices)
    {
        shared_ptr<Node> u = minHeap.popMin();
        C[u->getNumber() - 1] = true;
        visitedNodes++;
        vector<shared_ptr<Connection>> uConnections = u->getConnections();
        for (auto & uConnection : uConnections)
        {
            int otherNodeNumber = uConnection->getEndNode()->getNumber();
            if (!C[otherNodeNumber - 1])
            {
                shared_ptr<Node> v = uConnection->getEndNode();
                int w = uConnection->getWeight();
                if (D[v->getNumber() - 1] > D[u->getNumber() - 1] + w) 
                {
                    D[v->getNumber() - 1] = D[u->getNumber() - 1] + w;
                    A[v->getNumber() - 1] = u->getNumber() - 1;
                    minHeap.updatePriority(v, D[v->getNumber() - 1]);
                }
            }
        }

    }

    return make_tuple(D, A);
}

void UndirectedGraph::printDijkstra(int startNodeIndex, vector<int> D, vector<int> A)
{
    for (int i = 0; i < numberOfVertices; i++)
    {
        vector<int> path;
        int currentNodeIndex = i;
        while (currentNodeIndex != startNodeIndex)
        {
            path.insert(path.begin(), currentNodeIndex + 1);
            currentNodeIndex = A[currentNodeIndex];
        }
        path.insert(path.begin(), startNodeIndex + 1);

        if (i == startNodeIndex)
        {
            cout << path.back() << ": ";
        }
        else
        {
            cout << path.back() << ": " << startNodeIndex + 1 << ",";
        }

        for (long unsigned int j = 1; j < path.size() - 1; j++)
        {
            cout << path[j] << ",";
        }
        cout << path[path.size() - 1] << "; d=" << D[i] << endl;
    }
}

vector<vector<int>> UndirectedGraph::floydWarshall()
{
	vector<vector<int>> D;
    int numberOfVertices = this->numberOfVertices;
	for (int i = 0; i < numberOfVertices; i++) //Populating the Matrix D
    {
        vector<int> connections;
		for (int j = 0; j < numberOfVertices; j++) //verify if it is not actually just <= for all the other cases
        {
            tuple<bool,shared_ptr<Connection>> returnedValues = this->nodes[i]->getConnectionWith(this->nodes[j]);
            if (get<0>(returnedValues))
            {
                int weight = get<1>(returnedValues)->getWeight();
                connections.push_back(weight); 
            }
            else 
            {
                if (i==j)
                {
                    connections.push_back(0);
                }
                else
                {
                    connections.push_back(1000000);
                }
            }
		}
        D.push_back(connections); // should print this to verify correctness
	}

    for (int k = 0; k < numberOfVertices; k++) 
    {
        for (int i = 0; i < numberOfVertices; i++) 
        {
            for (int j = 0; j < numberOfVertices; j++) 
            {
                if (D[i][k] + D[k][j] < D[i][j]) {
                    D[i][j] = D[i][k] + D[k][j];
                }
            }
        }
    }

    return D;
}

void UndirectedGraph::printFloydWarshall(vector<vector<int>> D)
{
    for (int i = 0; i < numberOfVertices; i++)
    {
        cout << i + 1 << ": ";
        for (int j = 0; j < numberOfVertices; j++)
        {
            if (D[i][j] == 1000000)
            {
                cout << "inf" << " ";
            }
            else
            {
                cout << D[i][j] << " ";
            }
        }
        cout << endl;
    }
}

vector<int> UndirectedGraph::prim()
{
    int V = this->numberOfVertices;
    adj = new list<node_weight_pair>[V];

    for(auto conn:this->edges)
    { // Builds the adjacency matrix
        int i = conn->getStartNode()->getNumber() - 1;
        int j = conn->getEndNode()->getNumber() - 1;
        float w = conn->getWeight();
        adj[i].push_back(make_pair(j, w));
        adj[j].push_back(make_pair(i, w));
    }
    
    priority_queue< node_weight_pair, vector <node_weight_pair> , greater<node_weight_pair> > heap; // Heap as a priory queue
 
    int start_node = 0; // Vertex 0 as the arbitrary start node 
 
    vector<float> K(V, 1000000); // Vector to keep track of weight values
 
    vector<int> A(V, -1);
 
    vector<bool> visited(V, false);
 
    heap.push(make_pair(0, start_node)); // Initialization
    K[start_node] = 0;
 
    while (!heap.empty())
    {
        int u = heap.top().second; // Extract the minimum
        heap.pop();
        
        if(visited[u] == true)
        {
            continue;
        }

        visited[u] = true;  // Visited
 
        list< pair<int, float> >::iterator i;
        for (i = adj[u].begin(); i != adj[u].end(); ++i)
        {
            int v = (*i).first;
            float weight = (*i).second;

            if (visited[v] == false && K[v] > weight)
            {
                K[v] = weight;
                heap.push(make_pair(K[v], v));
                A[v] = u;
            }
        }
    }
    return A;
}

void UndirectedGraph::printPrim(vector<int> A)
{
    float weightSum = 0;
    string message;
    for (int i = 1; i < this->numberOfVertices; ++i){
        if (!message.empty())
            {
                message += ", ";
            }
        message += to_string(A[i]+1) + "-" + to_string(i+1);

        auto nodeOnTheOtherEnd = get<1>(this->nodes[i]->getConnectionWith(this->nodes[A[i]]));
        weightSum += nodeOnTheOtherEnd->getWeight();
    }
    cout << weightSum << endl;
    cout << message << endl;
}

vector<int>  UndirectedGraph::coloring()
{
    /* Crie um programa que recebe um grafo não-dirigido e não-ponderado como argumento. Ao final, informe:
    /  - [X] a coloração mínima e 
    /  - [X] qual número cromático foi utilizado em cada vertice */

    int V = this->numberOfVertices;
    color_adj = new list<int>[V]; // Adjacency list for coloring algorithm
    vector<int> colors; // Array to save the final results of the coloring

    for(auto conn:this->edges)
    { // Builds the adjacency matrix
        int i = conn->getStartNode()->getNumber() - 1;
        int j = conn->getEndNode()->getNumber() - 1;
        color_adj[i].push_back(j);
        color_adj[j].push_back(i);
    }
 
    for (int u = 0; u < V; u++)
    {  // Initialize values, only the first is 0 for now
        if (u == 0)
        {
            colors.push_back(0);
        }
        else
        {
            colors.push_back(-1);
        }
    }

    bool used_color[V]; // Array to indicate if color is already being used (True) by adjacent vertices
    for (int i = 0; i < V; i++)
    {
        used_color[i] = false;
    }

    for (int u = 1; u < V; u++)
    {  // Coloring process
        
        list<int>::iterator i;
        for (i = color_adj[u].begin(); i != color_adj[u].end(); ++i)
        { // Setting adjancent vertices colors as used
            if (colors[*i] != -1)
                used_color[colors[*i]] = true;
        }

        int color;
        for (color = 0; color < V; color++)
        { // Finds another color
            if (used_color[color] == false)
            {
                break;
            }
        }
        colors[u] = color;

        for (i = color_adj[u].begin(); i != color_adj[u].end(); ++i)
        { // Resetting for next iteration
            if (colors[*i] != -1)
            {
                used_color[colors[*i]] = false;
            }
        }
    }
    
    return colors;
} 

void UndirectedGraph::printColoring(vector<int> colors){
    int V = this->numberOfVertices;
    unordered_map <int, int> mp;

    for (int i = 0; i < V; i++){
        mp[colors[i]]++;
        cout << "Node " << i+1 << " = Color " << colors[i] +1 << endl;
    }
    
    cout<< "\nMinimum coloring equals "<< mp.size()<<endl;
    
}

void UndirectedGraph::configureBipartiteGraph()
{   
    m = domain.size();
    n = contradomain.size(); 
    adjacency = new list<int>[m+1];
    for(long unsigned int idx = 0; idx < this->edges.size(); idx++)
    { // Builds adjacency matrix
        if (idx%2==0){
            int i = this->edges[idx]->getStartNode()->getNumber();
            int j = this->edges[idx]->getEndNode()->getNumber();
            if (j%m == 0)
            {
                adjacency[i].push_back(m);
            }
            else 
            {
                adjacency[i].push_back(j%m);
            }
        } 
    }
}

tuple<int, int*> UndirectedGraph::hopcroftKarp()
{
    /* Crie um programa que receba um arquivo de grafo bipartido, nao-dirigido, nao-ponderado e informe:
    /  - [X] Qual o valor do emparelhamento máximo e 
    /  - [X] Quais arestas pertencem a ele. 
    /  Utilize o algoritmo de Hopcroft-Karp.*/

    configureBipartiteGraph();

    // matching of the domain
    pairU = new int[m+1];
 
    // matching of the counterdomain
    pairV = new int[n+1];
 
    // distance of domain
    dist = new int[m+1];
 
    // Initializing as 0 all vertices
    for (int u=0; u<=m; u++)
    {
        pairU[u] = 0; 
    }
    for (int v=0; v<=n; v++)
    {
        pairV[v] = 0;
    }
 
    int result = 0;
 
    // While there is an augmenting path.
    while (bipartiteGraphBFS())
    {
        for (int u=1; u<=m; u++)
        {
            if (pairU[u]==0 && bipartiteGraphDFS(u))
            { // If the vertex has no match and there is an augmenting path
                result++;
            }
        }
    }
    return make_tuple(result, pairU);
}

bool UndirectedGraph::bipartiteGraphDFS(int u)
{
    if (u != 0)
    {
        list<int>::iterator i;
        for (i=adjacency[u].begin(); i!=adjacency[u].end(); ++i)
        {
            int v = *i; // Adjacent to u
            if (dist[pairV[v]] == dist[u]+1)
            { // Distances results from BFS
                if (bipartiteGraphDFS(pairV[v]))
                { // If it is a match
                    pairV[v] = u;
                    pairU[u] = v;
                    return true;
                }
            }
        }
        dist[u] = 1000000; // If there is no augmenting path
        return false;
    }
    return true;
}

bool UndirectedGraph::bipartiteGraphBFS()
{
    queue<int> Q;
    for (int u=1; u<=m; u++)
    { // Initializing distance as 0
        if (pairU[u]==0)
        { // Free vertex, adds it to queue
            dist[u] = 0;
            Q.push(u);
        }
        else
        { // Else set distance as infinite so that this vertex is considered next time
            dist[u] = 1000000;
        }
    } 
    dist[0] = 1000000; // Initialize distance to 0 as infinite, since it's a void index for the adj matrix

    while (!Q.empty())
    {
        int u = Q.front();
        Q.pop();
        if (dist[u] < dist[0])
        { // If this node is not the void index and can provide a shorter path
            list<int>::iterator i;
            for (i=adjacency[u].begin(); i!=adjacency[u].end(); ++i)
            { // Gets all adjacents of u
                int v = *i;
                if (dist[pairV[v]] == 1000000)
                { // If match of v is not explored so far
                    dist[pairV[v]] = dist[u] + 1; 
                    Q.push(pairV[v]); // Adds it to queue
                }
            }
        }
    }
    // If back to void index by alternating path of distinct vertices then there is an augmenting path
    return (dist[0] != 1000000);
}

void UndirectedGraph::printHopcroftKarp(int maximum_matching, int* pairs)
{
    cout<< "Matching Size: " << maximum_matching <<endl;
    cout << "Edges: ";
    for (int i=1; i<=m; i++)
    {
        cout << i << "-" << pairs[i]+m << "\n       ";
    }
    cout<<endl;
}

UndirectedGraph::~UndirectedGraph()
= default;
