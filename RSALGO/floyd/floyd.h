#ifndef FLOYD_H
#define FLOYD_H

#include "../graph/graph.h"

// Structure pour stocker les résultats de Floyd-Warshall
typedef struct
{
    float **distance;  // Matrice des distances
    int **predecessor; // Matrice des prédecesseurs
    int nodeCount;     // Nombre de nœuds
} ShortestPaths;

ShortestPaths *findShortestPath(Graph *graph);
void seeShortestPath(ShortestPaths *sp, int u, int v, Node *nodes);
void freeShortestPaths(ShortestPaths *sp);

#endif