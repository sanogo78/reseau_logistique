#ifndef MULTI_DAY_PLANNER_H
#define MULTI_DAY_PLANNER_H
#include "../graph/graph.h"
#include "../temp/temp.h"
#include "../report/report.h"

void dynamicprog(Graph *graph, int start, int numDays, TimeContext context, const char *vehicle);
void printDetailedSolution(Graph *graph, int finalMask, int finalNode, int finalDay, TimeContext context, const char *vehicle);
#endif // DELIVERY_PLANNER_H