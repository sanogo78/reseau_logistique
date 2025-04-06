#ifndef TSP_H
#define TSP_H

// Représente un itinéraire de TSP
typedef struct
{
    int *path;       // Ordre des villes visitées
    float totalCost; // Distance totale du parcours
    int size;        // Nombre de villes
} TSPResult;

TSPResult *solveTSP(float **distanceMatrix, int cityCount);
void printTSPResult(TSPResult *result);
void freeTSPResult(TSPResult *result);

#endif
