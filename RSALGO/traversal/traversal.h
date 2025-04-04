#ifndef TRAVERSAL_H
#define TRAVERSAL_H

#include "../graph/graph.h"

typedef struct
{
    int nodeCount;
    int edgeCount;
    float averageDegree;
    int articulationPointsCount;
    int connectedComponents;
    bool isCyclic;
} NetworkStats;

NetworkStats calculateNetworkStats(Graph *graph);

int *depthFirstSearch(Graph *graph, int startNode, int *tree);
void printDFSTree(int *pi, int V, Node *nodes);
int *breadthFirstSearch(Graph *graph, int startNode, int *tree);
void printBFSTree(int *pi, int V, Node *nodes);

// cycle dans le reseau
bool hasCycle(Graph *graph);
bool isCyclicUtil(Graph *graph, int v, bool visited[], int parent);

// composant cornexe
void findConnectedComponents(Graph *graph);
void DFSUtil(Graph *graph, int v, bool visited[], int component[], int currentComponent);

// points d’articulation
void findArticulationPoints(Graph *graph);
void APUtil(Graph *graph, int u, bool visited[], int disc[], int low[], int parent[], bool ap[]);

// accessibilité entre différentes zones du réseau
bool isReachable(Graph *graph, int src, int dest);
#endif
