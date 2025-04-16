#include <stdlib.h>
#include <stdbool.h>
#include "crossing.h"

// Fonction utilitaire : vérifie si un gène est déjà présent dans le segment de l'enfant
bool isGeneInSegment(const Chromosome *child, int start, int end, int gene)
{
    for (int i = start; i <= end; i++)
    {
        if (child->genes[i] == gene)
        {
            return true;
        }
    }
    return false;
}

// Opérateur de croisement OX pour le VRP
void crossover(const Chromosome *parent1, const Chromosome *parent2, Chromosome *child,
               Graph *graph, Season season, TimePeriod timePeriod)
{
    int size = parent1->numNodes;
    int start = rand() % size;
    int end = start + (rand() % (size - start));

    for (int i = start; i <= end; i++)
    {
        child->genes[i] = parent1->genes[i];
    }

    int childPos = 0;
    for (int i = 0; i < size; i++)
    {
        if (childPos == start)
        {
            childPos = end + 1;
        }

        int gene = parent2->genes[i];
        if (!isGeneInSegment(child, start, end, gene))
        {
            child->genes[childPos++] = gene;
        }
    }

    if (!isValidChromosome(child, graph))
    {
        repairChromosome(child, graph, season, timePeriod);
    }
}

void repairChromosome(Chromosome *chrom, const Graph *graph, Season season, TimePeriod timePeriod)
{
    for (int r = 0; r < chrom->numRoutes; r++)
    {
        int start = chrom->routeBreaks[r];
        int end = (r == chrom->numRoutes - 1) ? chrom->numNodes : chrom->routeBreaks[r + 1];
        float currentTime = 0.0f; // Heure de départ du dépôt
        for (int i = start; i < end - 1; i++)
        {
            int from = chrom->genes[i];
            int to = chrom->genes[i + 1];

            if (!isTimeCompatible(graph, from, to, currentTime, season, timePeriod))
            {
                for (int j = i + 2; j < end; j++)
                {
                    int alternative = chrom->genes[j];
                    if (isTimeCompatible(graph, from, alternative, currentTime, season, timePeriod))
                    {
                        int temp = chrom->genes[i + 1];
                        chrom->genes[i + 1] = chrom->genes[j];
                        chrom->genes[j] = temp;
                        break;
                    }
                }
            }
            EdgeAttr attr;
            AdjListNode *node = graph->array[from].head;
            while (node != NULL)
            {
                if (node->dest == to)
                {
                    attr = node->attr;
                    break;
                }
                node = node->next;
            }
            currentTime = fmax(currentTime, graph->nodes[from].earliestTime) + graph->nodes[from].serviceTime + attr.baseTime;
        }
    }
}

bool isTimeCompatible(const Graph *graph, int from, int to, float currentTime, Season season, TimePeriod timePeriod)
{
    // 1. Vérifier si le nœud de départ est encore accessible
    if (currentTime > graph->nodes[from].latestTime)
    {
        return false; // Trop tard pour partir
    }

    // 2. Calculer le temps de trajet ajusté
    EdgeAttr attr;
    AdjListNode *node = graph->array[from].head;
    while (node != NULL)
    {
        if (node->dest == to)
        {
            attr = node->attr;
            break;
        }
        node = node->next;
    }

    float timeMultiplier = 1.0f;

    // Ajustement saisonnier
    if (season == SAISON_PLUVIEUSE)
        timeMultiplier += 0.2f; // +20% en saison pluvieuse

    // Ajustement horaire
    switch (timePeriod)
    {
    case HEURE_POINTE:
        timeMultiplier += attr.temporal.afternoon;
        break;
    case NUIT:
        timeMultiplier += attr.temporal.night;
        break;
    default:
        break; // HEURE_CREUSE: pas d'ajustement
    }

    float travelTime = attr.baseTime * timeMultiplier;

    // 3. Calculer l'heure d'arrivée
    float serviceTime = graph->nodes[from].serviceTime;
    float departureTime = fmax(currentTime, graph->nodes[from].earliestTime) + serviceTime;
    float arrivalTime = departureTime + travelTime;

    // 4. Vérifier la fenêtre de temps d'arrivée
    return (arrivalTime <= graph->nodes[to].latestTime);
}
