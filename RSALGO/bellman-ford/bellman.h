#ifnded BELLMAN_H
#define BELLMAN_H

#include "../graph/graph.h"

//contexte temporel (heure + saison)
typedef struct{
    TimePeriod TimePeriod;
    Season season;
}TimeContext;

//structure des résultats de bellman-ford
typedef struct{
    float *distance;
    int *predecessor;
    int nodeCount;
} bellmanResult;

//fonctions principales
bellmanResult* runBellmanFord(Graph *graph, int source, TimeContext ctx);
void showPathBellman(bellmanResult *result, int source, int target, node *nodes);
void freeBellmanResult(bellmanResult *result);

#endif