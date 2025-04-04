
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "../graph/graph.h"
#include "bellman.h"

// Calcule le poids ajusté selon le contexte temporel
float getAdjustedWeight(EdgeAttr attr, TimeContext ctx)
{
    if (!attr.temporal.seasonalAvailability[ctx.season])
    {
        return FLT_MAX; // Arc non disponible dans cette saison
    }

    float timeFactor = attr.temporal.timeOfDayMultipliers[ctx.timePeriod];
    float seasonFactor = attr.temporal.seasonMultipliers[ctx.season];

    return attr.baseTime * timeFactor * seasonFactor;
}

BellmanResult *runBellmanFord(Graph *graph, int source, TimeContext ctx)
{
    int V = graph->V;
    float *dist = malloc(V * sizeof(float));
    int *pred = malloc(V * sizeof(int));

    for (int i = 0; i < V; i++)
    {
        dist[i] = FLT_MAX;
        pred[i] = -1;
    }
    dist[source] = 0;

    for (int i = 1; i < V; i++)
    {
        for (int u = 0; u < V; u++)
        {
            AdjListNode *node = graph->array[u].head;
            while (node)
            {
                int v = node->dest;
                float w = getAdjustedWeight(node->attr, ctx);
                if (w != FLT_MAX && dist[u] != FLT_MAX && dist[u] + w < dist[v])
                {
                    dist[v] = dist[u] + w;
                    pred[v] = u;
                }
                node = node->next;
            }
        }
    }

    // Détection de cycle négatif
    for (int u = 0; u < V; u++)
    {
        AdjListNode *node = graph->array[u].head;
        while (node)
        {
            int v = node->dest;
            float w = getAdjustedWeight(node->attr, ctx);
            if (w != FLT_MAX && dist[u] != FLT_MAX && dist[u] + w < dist[v])
            {
                printf("Cycle de poids négatif détecté !\n");
                free(dist);
                free(pred);
                return NULL;
            }
            node = node->next;
        }
    }

    BellmanResult *result = malloc(sizeof(BellmanResult));
    result->distance = dist;
    result->predecessor = pred;
    result->nodeCount = V;
    return result;
}

void showPathBellman(BellmanResult *result, int source, int target, Node *nodes)
{
    if (result->distance[target] == FLT_MAX)
    {
        printf("\nAucun chemin entre %s (%d) et %s (%d)\n",
               nodes[source].name, source, nodes[target].name, target);
        return;
    }

    printf("\nChemin le plus court (Bellman-Ford) de %s (%d) à %s (%d):\n",
           nodes[source].name, source, nodes[target].name, target);
    printf("Temps total : %.2f minutes\n", result->distance[target]);

    // Reconstruction du chemin
    int path[result->nodeCount];
    int count = 0;
    int current = target;
    while (current != -1 && current != source)
    {
        path[count++] = current;
        current = result->predecessor[current];
    }
    path[count] = source;

    // Affichage
    printf("Itinéraire : ");
    for (int i = count; i >= 0; i--)
    {
        printf("%s (%d)", nodes[path[i]].name, path[i]);
        if (i > 0)
            printf(" -> ");
    }
    printf("\n");
}

void freeBellmanResult(BellmanResult *result)
{
    if (!result)
        return;
    free(result->distance);
    free(result->predecessor);
    free(result);
}
