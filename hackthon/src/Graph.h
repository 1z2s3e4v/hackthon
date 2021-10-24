#ifndef GRAPH_H
#define GRAPH_H

// A C++ program to print topological sorting of a DAG
#include <iostream>
#include <list>
#include <stack>
#include <vector>
using namespace std;
  
// Class to represent a graph
class Graph {
    int V; // No. of vertices'
  
    // Pointer to an array containing adjacency listsList
    list<int>* adj;
    list<int>* revAdj;
  
    // A function used by topologicalSort
    void topologicalSortUtil(int v, bool visited[], stack<int>& Stack);
  
public:
    Graph(int V); // Constructor
  
    // function to add an edge to graph
    void addEdge(int v, int w);

    list<int> parentOfNode(int v);
  
    // prints a Topological Sort of the complete graph
    vector<size_t> topologicalSort();
};

#endif