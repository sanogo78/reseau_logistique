#include "tsp.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <stdbool.h>

TSPResult *solveTSP(float **distanceMatrix, int cityCount)
{
    bool *visited = calloc(cityCount, sizeof(bool));
    int *path = malloc((cityCount + 1) * sizeof(int)); // +1 pour revenir au départ

    float totalCost = 0;
    int current = 0;
    path[0] = current;
    visited[current] = true;

    for (int step = 1; step < cityCount; step++)
    {
        int next = -1;
        float minDist = FLT_MAX;

        for (int i = 0; i < cityCount; i++)
        {
            if (!visited[i] && distanceMatrix[current][i] < minDist)
            {
                minDist = distanceMatrix[current][i];
                next = i;
            }
        }

        if (next == -1)
            break;

        path[step] = next;
        visited[next] = true;
        totalCost += minDist;
        current = next;
    }

    // Retour à la ville de départ
    totalCost += distanceMatrix[current][path[0]];
    path[cityCount] = path[0]; // boucle fermée

    free(visited);

    TSPResult *result = malloc(sizeof(TSPResult));
    result->path = path;
    result->totalCost = totalCost;
    result->size = cityCount + 1;

    return result;
}

void printTSPResult(TSPResult *result)
{
    printf("\nItineraire optimal approchee (plus proche voisin):\n");
    for (int i = 0; i < result->size; i++)
    {
        printf("Ville %d", result->path[i]);
        if (i < result->size - 1)
            printf(" -> ");
    }
    printf("\nDistance totale: %.2f\n", result->totalCost);
}

void freeTSPResult(TSPResult *result)
{
    if (!result)
        return;
    free(result->path);
    free(result);
}
