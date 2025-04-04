#include "floyd.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include "../graph/graph.h"

ShortestPaths *findShortestPath(Graph *graph)
{
    int V = graph->V;

    // Allocation de la matrice des distances
    float **dist = (float **)malloc(V * sizeof(float *));
    float **pred = (float **)malloc(V * sizeof(float *));
    for (int i = 0; i < V; i++)
    {
        dist[i] = (float *)malloc(V * sizeof(float *));
        pred[i] = (float *)malloc(V * sizeof(float *));

        for (int j = 0; j < V; j++)
        {
            if (i == j)
            {
                dist[i][j] = 0;
                pred[i][j] = -1;
            }
            else
            {
                dist[i][j] = FLT_MAX;
                pred[i][j] = -1;
            }
        }
    }

    // Initialisation avec les arêtes existantes
    for (int i = 0; i < V; i++)
    {
        AdjListNode *node = graph->array[i].head;
        while (node != NULL)
        {
            int v = node->dest;
            dist[i][v] = node->attr.baseTime; // Utilisation du temps de parcours comme poids
            pred[i][v] = i;
            node = node->next;
        }
    }

    // Application de l'algorithme de floyd-warshall
    for (int k = 0; k < graph->V; k++)
    {
        for (int i = 0; i < graph->V; i++)
        {
            for (int j = 0; j < graph->V; j++)
            {
                if (dist[i][k] != FLT_MAX && dist[k][j] != FLT_MAX && dist[i][j] > dist[i][k] + dist[k][j]) // Pour éviter les erreurs d'infini
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    pred[i][j] = pred[k][j];
                }
            }
        }
    }

    // Création de la structure de résultat
    ShortestPaths *sp = (ShortestPaths *)malloc(sizeof(ShortestPaths));
    sp->distance = dist;
    sp->predecessor = pred;
    sp->nodeCount = V;

    return sp;
}

void seeShortestPath(ShortestPaths *sp, int u, int v, Node *nodes)
{
    if (sp->distance[u][v] == FLT_MAX)
    {
        printf("\nAucun chemin entre %s (%d) et %s (%d)\n",
               nodes[u].name, u, nodes[v].name, v);
        return;
    }

    printf("\nChemin le plus court de %s (%d) a %s (%d):\n",
           nodes[u].name, u, nodes[v].name, v);
    printf("\nTemps total: %.2f minutes\n", sp->distance[u][v]);

    // Reconstruction du chemin
    int path[sp->nodeCount];
    int count = 0;
    int current = v;

    while (current != -1 && current != u)
    {
        path[count++] = current;
        current = sp->predecessor[u][current];
    }
    path[count] = u;

    // Affichage du chemin
    printf("Itineraire: ");
    for (int i = count; i >= 0; i--)
    {
        printf("%s (%d)", nodes[path[i]].name, path[i]);
        if (i > 0)
            printf(" -> ");
    }
    printf("\n");
}

void freeShortestPaths(ShortestPaths *sp)
{
    if (sp == NULL)
        return;

    for (int i = 0; i < sp->nodeCount; i++)
    {
        free(sp->distance[i]);
        free(sp->predecessor[i]);
    }
    free(sp->distance);
    free(sp->predecessor);
    free(sp);
}