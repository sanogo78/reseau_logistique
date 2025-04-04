#include "bellman.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

//poids ajusté selon le context temporel
float getAdjustedWeight(EdgeAttr attr, TimeContext ctx) {
    if (!attr.temporal.seasonalAvailability[ctx.season]){
        return FLT_MAX; //non disponible
    }

    BellmanResult/ runBellmanFord(Graph *graph, int source, TimeContext ctx) {
        int V = graph->V;
        float *dist = malloc(V * sizeof(float));

        for (int i = 0; i < V; i++) {
            dist[i] = FLT_MAX;
            pred[i] = -1;
        }
        dist[]
    }
}   