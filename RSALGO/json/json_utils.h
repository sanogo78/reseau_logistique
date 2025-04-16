#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "../graph/graph.h"

Graph *parseurFromJSON(const char *filename);
void saveNetworkState(Graph *graph, const char *filename);

#endif
