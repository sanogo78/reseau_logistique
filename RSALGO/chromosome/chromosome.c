#include "chromosome.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

// Crée un chromosome avec une permutation aléatoire
Chromosome *createRandomChromosome(int numNodes)
{
    Chromosome *chrom = (Chromosome *)malloc(sizeof(Chromosome));
    if (!chrom)
    {
        printf("Erreur d'allocation pour le chromosome");
        return NULL;
    }
    chrom->genes = (int *)malloc(numNodes * sizeof(int));
    if (!chrom->genes)
    {
        printf("Erreur d'allocation pour les gènes");
        free(chrom);
        return NULL;
    }
    chrom->numNodes = numNodes;
    chrom->totalCost = 0.0;
    chrom->totalTime = 0.0;
    chrom->totalDistance = 0.0;
    chrom->routeBreaks = NULL;
    chrom->numRoutes = 0;
    // Initialisation avec une permutation aléatoire
    for (int i = 0; i < numNodes; i++)
    {
        chrom->genes[i] = i;
    }
    for (int i = numNodes - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = chrom->genes[i];
        chrom->genes[i] = chrom->genes[j];
        chrom->genes[j] = temp;
    }
    return chrom;
}

// Découpe le chromosome en tournées selon une capacité maximale
void decodeChromosome(Chromosome *chrom, int max_capacity)
{
    if (max_capacity <= 0 || !chrom)
        return;

    // Réinitialiser les tournées précédentes
    if (chrom->routeBreaks)
        free(chrom->routeBreaks);

    // Estimation du nombre maximal de tournées
    int max_routes = (chrom->numNodes / max_capacity) + 2;
    chrom->routeBreaks = (int *)malloc(max_routes * sizeof(int));
    chrom->numRoutes = 0;
    chrom->routeBreaks[0] = 0; // Première tournée commence à l'indice 0

    int current_route_size = 0;
    for (int i = 0; i < chrom->numNodes; i++)
    {
        current_route_size++;
        if (current_route_size >= max_capacity && i < chrom->numNodes - 1)
        {
            chrom->numRoutes++;
            chrom->routeBreaks[chrom->numRoutes] = i + 1;
            current_route_size = 0;
        }
    }
    chrom->numRoutes++; // Dernière tournée
}

// Vérifie si le chromosome est valide (pas de doublons, liaisons existantes)
int isValidChromosome(Chromosome *chrom, Graph *graph)
{
    if (!chrom || !graph)
        return false;

    // Vérifie les doublons
    for (int i = 0; i < chrom->numNodes; i++)
    {
        for (int j = i + 1; j < chrom->numNodes; j++)
        {
            if (chrom->genes[i] == chrom->genes[j])
            {
                printf("Erreur : doublon du noeud %d\n", chrom->genes[i]);
                return false;
            }
        }
    }

    // Vérifie les liaisons entre noeuds consécutifs
    for (int i = 0; i < chrom->numNodes - 1; i++)
    {
        int from = chrom->genes[i];
        int to = chrom->genes[i + 1];
        if (!edgeExists(graph, from, to))
        {
            printf("Erreur : pas de liaison entre %d et %d\n", from, to);
            return false;
        }
    }
    return true;
}

// Calcule le coût, temps, et distance
void ChromosomeCost(Chromosome *chrom, Graph *graph, Season season, TimePeriod time)
{
    if (!chrom || !graph)
        return;

    chrom->totalCost = 0.0;
    chrom->totalTime = 0.0;
    chrom->totalDistance = 0.0;

    for (int i = 0; i < chrom->numNodes - 1; i++)
    {
        int from = chrom->genes[i];
        int to = chrom->genes[i + 1];
        EdgeAttr attr = getEdgeAttr(graph, from, to);

        // Applique les coefficients saison/horaire
        float timeMultiplier = 1.0;
        if (season == SAISON_PLUVIEUSE)
            timeMultiplier += 0.2;
        if (time == HEURE_POINTE)
            timeMultiplier += attr.temporal.afternoon;

        chrom->totalDistance += attr.distance;
        chrom->totalTime += attr.baseTime * timeMultiplier;
        chrom->totalCost += attr.cost;
    }
}

// Affiche le chromosome et ses tournées
void printChromosome(const Chromosome *chrom, Graph *graph)
{
    if (!chrom || !graph)
        return;

    printf("Itineraire complet : ");
    for (int i = 0; i < chrom->numNodes; i++)
    {
        printf("%s", graph->nodes[chrom->genes[i]].name);
        if (i < chrom->numNodes - 1)
            printf(" -> ");
    }
    printf("\n");

    if (chrom->routeBreaks)
    {
        printf("Tournees decoupees :\n");
        for (int r = 0; r < chrom->numRoutes; r++)
        {
            int start = chrom->routeBreaks[r];
            int end = (r == chrom->numRoutes - 1) ? chrom->numNodes : chrom->routeBreaks[r + 1];
            printf("  Tournee %d : ", r + 1);
            for (int i = start; i < end; i++)
            {
                printf("%s", graph->nodes[chrom->genes[i]].name);
                if (i < end - 1)
                    printf(" -> ");
            }
            printf("\n");
        }
    }
}
bool edgeExists(Graph *graph, int from, int to)
{
    if (from >= graph->V || to >= graph->V || from < 0 || to < 0)
    {
        return false;
    }
    return (graph->distance >= 0); // Vérifie si l'arête existe
}

// Libère la mémoire
void freeChromosome(Chromosome *chrom)
{
    if (!chrom)
        return;
    if (chrom->genes)
        free(chrom->genes);
    if (chrom->routeBreaks)
        free(chrom->routeBreaks);
    free(chrom);
}