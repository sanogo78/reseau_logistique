#ifndef BELLMAN_H
#define BELLMAN_H

#include "../graph/graph.h"

// Contexte temporel (heure + saison)
typedef struct {
    TimePeriod timePeriod;
    Season season;
} TimeContext;

// Structure des résultats de Bellman-Ford
typedef struct {
    float *distance;
    int *predecessor;
    int nodeCount;
} BellmanResult;

// Fonctions principales
BellmanResult* runBellmanFord(Graph *graph, int source, TimeContext ctx);
void showPathBellman(BellmanResult *result, int source, int target, Node *nodes);
void freeBellmanResult(BellmanResult *result);

#endif
