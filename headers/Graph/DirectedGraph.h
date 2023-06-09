#ifndef DirectedGraph_h
#define DirectedGraph_h

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <stack>
#include <memory>
#include "Graph/AbstractGraph.h"

using namespace std;

namespace GraphModule
{
    class Node;
    class Connection;

    class DirectedGraph : public AbstractGraph
    {
        public:
            DirectedGraph();
        
            int getDegreeOfNode(int nodeKey) override;

            int getInDegreeOfNode(int nodeKey);

            int getOutDegreeOfNode(int nodeKey);

            void stronglyConnectedComponents();

            void DFS(int v, bool C[], stack<int> &Stack);

            void DFSVisit(int v, bool C[]);

            unique_ptr<DirectedGraph> BuildTransposedGraph();

            vector<shared_ptr<Node>> topologicalSorting();

            void DFSVisitTopologicalSorting(int v, vector<bool> &C, vector<int> &F, vector<int> &T, vector<shared_ptr<Node>> &O, int &time);

            void printTopologicalSorting(const vector<shared_ptr<Node>> &O);

            void addArc(const shared_ptr<Node> &startNode, const shared_ptr<Node> &endNode, float weight);

            int edmondsKarp(int beginNodeIndex, int endNodeIndex);

            int BFS(int beginNodeIndex, int endNodeIndex, int parent[]);

            void printEdmontsKarp(int max_flow);

            ~DirectedGraph() override;

        private:
            int numberOfArcs;
            vector<shared_ptr<Connection>> arcs;
            list<int> *adj;
            vector<vector<float>> residualNetwork; // Residual Graph Adjacency matrix for edmondsKarp
    };
}

#endif