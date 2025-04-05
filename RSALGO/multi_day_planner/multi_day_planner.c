#include "multi_day_planner.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define MAX_DAYS 30

// Génère toutes les combinaisons possibles de villes à livrer (bitmasking)
int countBits(int mask) {
    int count = 0;
    while (mask) {
        count += mask & 1;
        mask >>= 1;
    }
    return count;
}

// Calcule le coût d’une tournée depuis le dépôt
float tourCost(float **dist, int depot, int *cities, int size) {
    if (size == 0) return 0;
    float cost = 0;
    int current = depot;

    for (int i = 0; i < size; i++) {
        cost += dist[current][cities[i]];
        current = cities[i];
    }

    cost += dist[current][depot]; // retour au dépôt
    return cost;
}

// Extraire les villes d'un mask (bitmask)
void maskToCities(int mask, int depot, int *cities, int *count) {
    *count = 0;
    for (int i = 0; i < 32; i++) {
        if (mask & (1 << i)) {
            if (i != depot) {
                cities[(*count)++] = i;
            }
        }
    }
}

// Programmation dynamique sur les sous-ensembles
MultiDayPlan *planMultiDayDeliveries(float **dist, int cityCount, int depot, int maxPerDay) {
    int totalComb = 1 << cityCount; // 2^n combinaisons
    float *dp = malloc(totalComb * sizeof(float));
    int *prev = malloc(totalComb * sizeof(int));
    int *used = malloc(totalComb * sizeof(int));

    for (int i = 0; i < totalComb; i++) {
        dp[i] = FLT_MAX;
        prev[i] = -1;
    }

    dp[0] = 0; // Pas de ville = coût 0

    // Prétraiter tous les groupes possibles livrables en 1 jour
    for (int mask = 1; mask < totalComb; mask++) {
        if (!(mask & (1 << depot))) { // on ignore si le dépôt est inclus
            int cities[32], size;
            maskToCities(mask, depot, cities, &size);
            if (size <= maxPerDay) {
                float cost = tourCost(dist, depot, cities, size);
                for (int sub = mask; sub; sub = (sub - 1) & mask) {
                    if (dp[mask ^ sub] + cost < dp[mask]) {
                        dp[mask] = dp[mask ^ sub] + cost;
                        prev[mask] = mask ^ sub;
                        used[mask] = sub;
                    }
                }
            }
        }
    }

    // Reconstruction du chemin
    int current = totalComb - 1;
    MultiDayPlan *plan = malloc(sizeof(MultiDayPlan));
    plan->days = malloc(MAX_DAYS * sizeof(int *));
    plan->sizes = malloc(MAX_DAYS * sizeof(int));
    plan->totalCost = dp[current];

    int day = 0;
    while (current > 0 && day < MAX_DAYS) {
        int sub = used[current];
        int *cities = malloc(cityCount * sizeof(int));
        int size;
        maskToCities(sub, depot, cities, &size);
        plan->days[day] = cities;
        plan->sizes[day] = size;
        current = prev[current];
        day++;
    }

    plan->totalDays = day;

    // Libération mémoire temporaire
    free(dp);
    free(prev);
    free(used);

    return plan;
}

void printMultiDayPlan(MultiDayPlan *plan) {
    printf("\n--- Planification multi-jours ---\n");
    for (int i = plan->totalDays - 1; i >= 0; i--) {
        printf("Jour %d : ", plan->totalDays - i);
        for (int j = 0; j < plan->sizes[i]; j++) {
            printf("Ville %d", plan->days[i][j]);
            if (j < plan->sizes[i] - 1) printf(" -> ");
        }
        printf("\n");
    }
    printf("Cout total : %.2f\n", plan->totalCost);
}

void freeMultiDayPlan(MultiDayPlan *plan) {
    for (int i = 0; i < plan->totalDays; i++) {
        free(plan->days[i]);
    }
    //liberation de memoire
    free(plan->days);
    free(plan->sizes);
    free(plan);
}
