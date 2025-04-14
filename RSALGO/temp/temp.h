#ifndef TEMP_H
#define TEMP_H

#include <stdbool.h>
#include "../graph/graph.h"

typedef struct
{
    int hour;             // Heure du jour
    int season;           // 0 = saison sèche, 1 = pluie
    float congestionRate; // Taux de congestion (ex : 0.7 pour 70% de congestion)
} TimeContext;

float adjustedTime(EdgeAttr attr, TimeContext ctx);

#endif