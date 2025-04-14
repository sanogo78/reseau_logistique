#include "report.h"
#include <stdio.h>
#include <string.h>

RouteAvoided avoidedRoutes[100];
int avoidedCount = 0;

void addAvoidedRoute(int from, int to, EdgeAttr attr)
{
    RouteAvoided r;
    r.from = from;
    r.to = to;

    if (attr.reliability < 0.5)
    {
        strcpy(r.reason, "Fiabilité faible");
    }
    else if (attr.restrictions & 1)
    {
        strcpy(r.reason, "Route interdite");
    }
    else
    {
        strcpy(r.reason, "Indisponible");
    }

    avoidedRoutes[avoidedCount++] = r;
}
