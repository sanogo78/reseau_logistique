#include <stdlib.h>
#include <stdbool.h>
#include "mutation.h"

void swapGenes(Chromosome *chrom, int i, int j)
{
    int temp = chrom->genes[i];
    chrom->genes[i] = chrom->genes[j];
    chrom->genes[j] = temp;
}

void swapMutation(Chromosome *chrom, Graph *graph)
{
    if (chrom->numRoutes == 0)
        return;

    // Choisir une tournée aléatoire
    int routeIdx = rand() % chrom->numRoutes;
    int start = chrom->routeBreaks[routeIdx];
    int end = (routeIdx == chrom->numRoutes - 1) ? chrom->numNodes : chrom->routeBreaks[routeIdx + 1];

    // Choisir deux positions distinctes
    int pos1 = start + rand() % (end - start);
    int pos2 = start + rand() % (end - start);
    if (pos1 == pos2)
        return;

    swapGenes(chrom, pos1, pos2);
}

void inversionMutation(Chromosome *chrom, Graph *graph)
{
    if (chrom->numRoutes == 0)
        return;

    int routeIdx = rand() % chrom->numRoutes;
    int start = chrom->routeBreaks[routeIdx];
    int end = (routeIdx == chrom->numRoutes - 1) ? chrom->numNodes : chrom->routeBreaks[routeIdx + 1];

    // Choisir un segment de 2+ gènes
    int segmentStart = start + rand() % ((end - start) / 2);
    int segmentEnd = segmentStart + 1 + rand() % (end - segmentStart - 1);

    // Inverser le segment
    while (segmentStart < segmentEnd)
    {
        swapGenes(chrom, segmentStart++, segmentEnd--);
    }
}

void adaptiveMutation(Chromosome *chrom, Graph *graph, Season season, TimePeriod timePeriod)
{
    float mutationType = (float)rand() / RAND_MAX;

    if (mutationType < 0.6)
    { // 60% swap mutation
        swapMutation(chrom, graph);
    }
    else if (mutationType < 0.9)
    { // 30% inversion mutation
        inversionMutation(chrom, graph);
    }
    else
    { // 10% mutation spéciale
        // Mutation ciblée (ex: échange inter-tournées)
        if (chrom->numRoutes > 1)
        {
            int route1 = rand() % chrom->numRoutes;
            int route2 = (route1 + 1 + rand() % (chrom->numRoutes - 1)) % chrom->numRoutes;

            int pos1 = chrom->routeBreaks[route1] + rand() % ((route1 == chrom->numRoutes - 1 ? chrom->numNodes : chrom->routeBreaks[route1 + 1]) - chrom->routeBreaks[route1]);

            int pos2 = chrom->routeBreaks[route2] + rand() % ((route2 == chrom->numRoutes - 1 ? chrom->numNodes : chrom->routeBreaks[route2 + 1]) - chrom->routeBreaks[route2]);

            swapGenes(chrom, pos1, pos2);
        }
    }

    repairChromosome(chrom, graph, season, timePeriod);
}

Chromosome *copyChromosome(const Chromosome *src)
{
    Chromosome *dest = createRandomChromosome(src->numNodes);
    memcpy(dest->genes, src->genes, src->numNodes * sizeof(int));
    // Copier tous les champs...
    dest->totalCost = src->totalCost;
    dest->numRoutes = src->numRoutes;
    if (src->routeBreaks)
    {
        dest->routeBreaks = malloc(src->numRoutes * sizeof(int));
        memcpy(dest->routeBreaks, src->routeBreaks, src->numRoutes * sizeof(int));
    }
    return dest;
}
void printMutation(const Chromosome *chrom, const Graph *graph)
{
    printf("Itineraire complet (%d tournees):\n", chrom->numRoutes);

    for (int r = 0; r < chrom->numRoutes; r++)
    {
        int start = chrom->routeBreaks[r];
        int end = (r == chrom->numRoutes - 1) ? chrom->numNodes : chrom->routeBreaks[r + 1];

        printf(" Tournee %d: ", r + 1);
        for (int i = start; i < end; i++)
        {
            printf("%s", graph->nodes[chrom->genes[i]].name);
            if (i < end - 1)
                printf(" -> ");
        }
        printf("\n");
    }
}