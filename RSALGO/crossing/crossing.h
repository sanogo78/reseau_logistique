#ifndef CROSING_H
#define CROSSING_H

#include "../graph/graph.h"
#include "../chromosome/chromosome.h"

bool isGeneInSegment(const Chromosome *child, int start, int end, int gene);
void crossoverOX(const Chromosome *parent1, const Chromosome *parent2, Chromosome *child, Graph *graph, Season season, TimePeriod timePeriod);
bool isTimeCompatible(const Graph *graph, int from, int to, float currentTime, Season season, TimePeriod timePeriod);

#endif