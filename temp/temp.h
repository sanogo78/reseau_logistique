#ifndef TEMP_H
#define TEMP_H

#include <stdbool.h>
#include "../graph/graph.h"

EdgeAttr getCurrentEdgeAttributes(EdgeAttr baseAttr);
void setTimeVariation(EdgeAttr *attr, TimePeriod period, float timeMultiplier);
void setSeasonVariation(EdgeAttr *attr, Season season, float reliabilityMultiplier, bool available);
#endif