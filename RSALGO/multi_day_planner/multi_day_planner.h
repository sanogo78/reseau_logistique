#ifndef MULTI_DAY_PLANNER_H
#define MULTI_DAY_PLANNER_H

typedef struct
{
    int **days;      // Tableau [jour][villes du jour]
    int *sizes;      // Nombre de villes par jour
    int totalDays;   // Nombre de jours
    float totalCost; // Coût total
} MultiDayPlan;

MultiDayPlan *planMultiDayDeliveries(float **distanceMatrix, int cityCount, int depot, int maxPerDay);
void printMultiDayPlan(MultiDayPlan *plan);
void freeMultiDayPlan(MultiDayPlan *plan);

#endif
