#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include "../graph/graph.h"

typedef struct
{
    int *genes;          // Séquence des nœuds visités (ex: [2, 0, 1, 3])
    int numNodes;        // Nombre total de nœuds (clients)
    float totalCost;     // Coût total (distance + pénalités)
    float totalTime;     // Temps total (inclut les délais)
    float totalDistance; // Distance totale
    int *routeBreaks;    // Indices où une nouvelle tournée commence (ex: [0, 2] pour 2 tournées)
    int numRoutes;       // Nombre de tournées
} Chromosome;

// Fonctions
Chromosome *createRandomChromosome(int numNodes);
void decodeChromosome(Chromosome *chrom, int max_capacity);
void ChromosomeCost(Chromosome *chrom, Graph *graph, Season season, TimePeriod time);
void printChromosome(const Chromosome *chrom, Graph *graph);
void freeChromosome(Chromosome *chrom);
int isValidChromosome(Chromosome *chrom, Graph *graph); // Nouvelle fonction
bool edgeExists(Graph *graph, int from, int to);

#endif