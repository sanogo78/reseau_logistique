#ifndef BELLMAN_H
#define BELLMAN_H

#include "../graph/graph.h"

typedef struct
{
    float *distance;  // Distances depuis la source
    int *predecessor; // Prédecesseurs
    int nodeCount;
} BellmanResult;

BellmanResult *findShortestPathBellman(Graph *graph, int source);
void seeBellmanPath(BellmanResult *br, int src, int dest, Node *nodes);
void freeBellmanResult(BellmanResult *br);

#endif
