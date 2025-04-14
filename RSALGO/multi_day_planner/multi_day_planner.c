#include "multi_day_planner.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#define MAXN 16
#define MAXDAYS 10
#define INF 1e9

float dp[1 << MAXN][MAXN][MAXDAYS]; // Tableaux dynamiques pour DP

int parent[1 << MAXN][MAXN][MAXDAYS]; // Pour enregistrer les parents dans la solution

float costAccum[1 << MAXN][MAXN][MAXDAYS]; // Accumulation des coûts
float timeAccum[1 << MAXN][MAXN][MAXDAYS]; // Accumulation du temps                   // Nombre de routes évitées

void dynamicprog(Graph *graph, int start, int numDays, TimeContext context, const char *vehicle)
{
    int N = graph->V;
    for (int i = 0; i < (1 << N); i++)
        for (int j = 0; j < N; j++)
            for (int d = 0; d < numDays; d++)
            {
                dp[i][j][d] = INF;
                costAccum[i][j][d] = 0;
                timeAccum[i][j][d] = 0;
            }

    dp[1 << start][start][0] = 0;

    for (int mask = 0; mask < (1 << N); mask++)
    {
        for (int u = 0; u < N; u++)
        {
            for (int day = 0; day < numDays - 1; day++)
            {
                if (dp[mask][u][day] == INF)
                    continue;

                AdjListNode *edge = graph->array[u].head;
                while (edge)
                {
                    int v = edge->dest;
                    if (mask & (1 << v))
                    {
                        edge = edge->next;
                        continue;
                    }

                    float time = adjustedTime(edge->attr, context);
                    float cost = edge->attr.cost;
                    float reliability = edge->attr.reliability;
                    int restrictions = edge->attr.restrictions;

                    if (time < 0 || reliability < 0.5 || (restrictions & 1))
                    {
                        addAvoidedRoute(u, v, edge->attr);
                        edge = edge->next;
                        continue;
                    }

                    float penalty = (1.0 - reliability) * 10.0;
                    float weightedCost = time + cost + penalty;

                    int newMask = mask | (1 << v);
                    float newTotal = dp[mask][u][day] + weightedCost;

                    if (dp[newMask][v][day + 1] > newTotal)
                    {
                        dp[newMask][v][day + 1] = newTotal;
                        parent[newMask][v][day + 1] = u;
                        costAccum[newMask][v][day + 1] = costAccum[mask][u][day] + cost;
                        timeAccum[newMask][v][day + 1] = timeAccum[mask][u][day] + time;
                    }

                    edge = edge->next;
                }
            }
        }
    }

    // === Affichage automatique de la meilleure solution ici ===
    int finalMask = -1, finalNode = -1, finalDay = -1;
    float bestCost = INF;

    for (int mask = 0; mask < (1 << N); mask++)
    {
        for (int node = 0; node < N; node++)
        {
            for (int day = 0; day < numDays; day++)
            {
                if (dp[mask][node][day] < bestCost && __builtin_popcount(mask) == N)
                {
                    bestCost = dp[mask][node][day];
                    finalMask = mask;
                    finalNode = node;
                    finalDay = day;
                }
            }
        }
    }

    if (finalMask != -1)
    {
        printDetailedSolution(graph, finalMask, finalNode, finalDay, context, vehicle);
    }
    else
    {
        printf("Aucune solution complete trouvee pour atteindre tous les points.\n");
    }
}

void printDetailedSolution(Graph *graph, int finalMask, int finalNode, int finalDay, TimeContext context, const char *vehicle)
{
    if (finalMask == 0)
        return;

    printf("=== SOLUTION OPTIMALE ===\n");
    printf("Saison: %s\n", context.season == 0 ? "saison_seche" : "saison_des_pluies");
    printf("Vehicule: %s\n", vehicle);
    printf("Cout total: %.2f FCFA\n", costAccum[finalMask][finalNode][finalDay]);
    printf("Duree totale: %d jours\n\n", finalDay);

    int path[MAXN], cnt = 0;
    int mask = finalMask, node = finalNode, day = finalDay;
    while (mask)
    {
        path[cnt++] = node;
        int prev = parent[mask][node][day];
        mask ^= (1 << node);
        node = prev;
        day--;
    }

    for (int i = cnt - 1, jour = 1; i > 0; i--, jour++)
    {
        int from = path[i], to = path[i - 1];
        printf("JOUR %d:\n", jour);
        printf("  - Depart: %s (%.6f, %.6f)\n", graph->nodes[from].name, graph->nodes[from].coordinates[0], graph->nodes[from].coordinates[1]);

        AdjListNode *edge = graph->array[from].head;
        while (edge && edge->dest != to)
            edge = edge->next;

        if (edge)
        {
            printf("  -> %s (%.1f km)%s\n", graph->nodes[to].name, edge->attr.distance,
                   (edge->attr.restrictions & 2) ? " [Fragile]" : "");
            printf("     Temps: %.1fh | Cout: %.0f FCFA\n", edge->attr.baseTime / 60.0, edge->attr.cost);
        }

        printf("  COUT JOURNALIER: %.0f FCFA\n\n", costAccum[1 << path[i - 1]][path[i - 1]][jour]);
    }

    if (avoidedCount > 0)
    {
        printf("ROUTES EVITEES:\n");
        for (int i = 0; i < avoidedCount; i++)
        {
            printf("  - %s -> %s (%s)\n", graph->nodes[avoidedRoutes[i].from].name,
                   graph->nodes[avoidedRoutes[i].to].name,
                   avoidedRoutes[i].reason);
        }
    }
}