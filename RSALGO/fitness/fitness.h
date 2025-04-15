#ifndef CROSING_H
#define CROSSING_H

#include "../graph/graph.h"
#include "../chromosome/chromosome.h"

float timeMultiplier(Season s, TimePeriod p, EdgeAttr attr);
float evaluateFitness(const Chromosome *chrom, const Graph *graph, Season season, TimePeriod timePeriod);
int UnusedVehicles(const Chromosome *chrom, const Graph *graph);
float RouteImbalance(const Chromosome *chrom, Graph *graph);
Chromosome *findBestSolution(Chromosome **pop, int size);
#endif