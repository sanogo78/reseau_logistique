#include <stdlib.h>
#include <stdbool.h>
#include "fitness.h"
float evaluateFitness(const Chromosome *chrom, const Graph *graph, Season season, TimePeriod timePeriod)
{
    const float alpha = 0.4f; // Poids du coût total
    const float beta = 0.3f;  // Poids du temps
    const float gamma = 0.2f; // Poids de la distance
    const float delta = 0.1f; // Poids secondaires

    float fitness = 0.0f;
    float totalPenalties = 0.0f;

    ChromosomeCost(chrom, graph, season, timePeriod);

    float normCost = chrom->totalCost / 5000.0f;
    float normTime = chrom->totalTime / 60.0f;
    float normDistance = chrom->totalDistance / 50.0f;

    fitness += alpha * normCost;
    fitness += beta * normTime;
    fitness += gamma * normDistance;

    for (int r = 0; r < chrom->numRoutes; r++)
    {
        int start = chrom->routeBreaks[r];
        int end = (r == chrom->numRoutes - 1) ? chrom->numNodes : chrom->routeBreaks[r + 1];
        int routeSize = end - start;

        if (routeSize > graph->nodes->capacity)
        {
            float excess = routeSize - graph->nodes->capacity;
            totalPenalties += 10.0f * excess; // Pénalité progressive
        }

        float currentTime = 0.0f;

        for (int i = start; i < end; i++)
        {
            int nodeId = chrom->genes[i];
            Node *node = &graph->nodes[nodeId];
            if (currentTime < node->earliestTime)
            {
                currentTime = node->earliestTime;
            }
            else if (currentTime > node->latestTime)
            {
                float delay = currentTime - node->latestTime;
                totalPenalties += 5.0f * delay;
            }

            currentTime += node->serviceTime;
            if (i < end - 1)
            {
                EdgeAttr attr = getEdgeAttr(graph, nodeId, chrom->genes[i + 1]);
                currentTime += attr.baseTime * timeMultiplier(season, timePeriod, attr);
            }
        }
    }
    fitness += delta * (0.3f * UnusedVehicles(chrom, graph) + 0.7f * RouteImbalance(chrom, graph));
    fitness += totalPenalties;

    return fitness;
}
float timeMultiplier(Season s, TimePeriod p, EdgeAttr attr)
{
    float multiplier = 1.0f;
    if (s == SAISON_PLUVIEUSE)
        multiplier += 0.2f;
    switch (p)
    {
    case HEURE_POINTE:
        multiplier += attr.temporal.afternoon;
        break;
    case NUIT:
        multiplier += attr.temporal.night;
        break;
    default:
        break;
    }
    return multiplier;
}

int UnusedVehicles(const Chromosome *chrom, const Graph *graph)
{
    return 2 - chrom->numRoutes;
}

float RouteImbalance(const Chromosome *chrom, Graph *graph)
{
    float avg = chrom->totalDistance / chrom->numRoutes;
    float imbalance = 0.0f;
    for (int r = 0; r < chrom->numRoutes; r++)
    {
        int start = chrom->routeBreaks[r];
        int end = (r == chrom->numRoutes - 1) ? chrom->numNodes : chrom->routeBreaks[r + 1];
        float routeDist = 0.0f;
        for (int i = start; i < end - 1; i++)
        {
            int from = chrom->genes[i];
            int to = chrom->genes[i + 1];
            EdgeAttr attr = getEdgeAttr(graph, from, to);
            routeDist += attr.distance; // ou attr.distance si disponibl
        }
        imbalance += fabs(routeDist - avg);
    }
    return imbalance;
}
Chromosome *findBestSolution(Chromosome **pop, int size)
{
    Chromosome *best = pop[0];
    for (int i = 1; i < size; i++)
    {
        if (pop[i]->totalCost < best->totalCost)
        {
            best = pop[i];
        }
    }
    return best;
}

// Sélection par tournoi
Chromosome *selectParent(Chromosome **pop, int size)
{
    int a = rand() % size;
    int b = rand() % size;
    return pop[a]->totalCost < pop[b]->totalCost ? pop[a] : pop[b];
}