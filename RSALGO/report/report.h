#ifndef REPORT_H
#define REPORT_H
#include "../graph/graph.h"
typedef struct
{
    int from;
    int to;
    char reason[100];
} RouteAvoided;

extern RouteAvoided avoidedRoutes[100];
extern int avoidedCount;

void addAvoidedRoute(int from, int to, EdgeAttr attr);

#endif
