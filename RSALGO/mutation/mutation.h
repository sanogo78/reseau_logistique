#ifndef MUTATION_H
#define MUTATION_H

#include "../graph/graph.h"
#include "../chromosome/chromosome.h"

void swapGenes(Chromosome *chrom, int i, int j);
void swapMutation(Chromosome *chrom, Graph *graph);
void inversionMutation(Chromosome *chrom, Graph *graph);
void adaptiveMutation(Chromosome *chrom, Graph *graph, Season season, TimePeriod timePeriod);
Chromosome *copyChromosome(const Chromosome *src);
void printMutation(const Chromosome *chrom, const Graph *graph);
#endif