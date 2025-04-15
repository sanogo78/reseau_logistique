#include "bellman.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>

BellmanResult *findShortestPathBellman(Graph *graph, int source)
{
    int V = graph->V;
    float *distance = malloc(V * sizeof(float));
    int *predecessor = malloc(V * sizeof(int));

    for (int i = 0; i < V; i++)
    {
        distance[i] = FLT_MAX;
        predecessor[i] = -1;
    }
    distance[source] = 0;
    // Relaxation |V| - 1 fois
    for (int i = 1; i < V; i++)
    {
        for (int u = 0; u < V; u++)
        {
            AdjListNode *node = graph->array[u].head;
            while (node != NULL)
            {
                int v = node->dest;
                float w = node->attr.baseTime;

                if (w != FLT_MAX)
                {
                    float newDistance = distance[u] + w;
                    if (newDistance < distance[v])
                    {
                        distance[v] = newDistance;
                        predecessor[v] = u;
                    }
                }

                node = node->next;
            }
        }
    }
    // Détection de cycle négatif
    for (int u = 0; u < V; u++)
    {
        AdjListNode *node = graph->array[u].head;
        while (node != NULL)
        {
            int v = node->dest;
            float w = node->attr.baseTime;

            if (distance[u] != FLT_MAX && distance[u] + w < distance[v])
            {
                printf("Le graphe contient un cycle de poids negatif.\n");
                free(distance);
                free(predecessor);
                return NULL;
            }
            node = node->next;
        }
    }
    BellmanResult *br = malloc(sizeof(BellmanResult));
    br->distance = distance;
    br->predecessor = predecessor;
    br->nodeCount = V;
    return br;
}

void seeBellmanPath(BellmanResult *br, int src, int dest, Node *nodes)
{
    if (br->distance[dest] == FLT_MAX)
    {
        printf("\nAucun chemin de %s (%d) vers %s (%d)\n", nodes[src].name, src, nodes[dest].name, dest);
        return;
    }

    printf("\nChemin de %s (%d) a %s (%d):\n", nodes[src].name, src, nodes[dest].name, dest);
    printf("Temps total : %.2f minutes\n", br->distance[dest]);

    int path[br->nodeCount];
    int count = 0;
    int current = dest;

    while (current != -1 && current != src)
    {
        path[count++] = current;
        current = br->predecessor[current];
    }
    path[count] = src;

    printf("Itineraire : ");
    for (int i = count; i >= 0; i--)
    {
        printf("%s (%d)", nodes[path[i]].name, path[i]);
        if (i > 0)
            printf(" -> ");
    }
    printf("\n");
}

void freeBellmanResult(BellmanResult *br)
{
    if (!br)
        return;
    free(br->distance);
    free(br->predecessor);
    free(br);
}
