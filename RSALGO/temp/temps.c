#include "../graph/graph.h"
#include <time.h>
#include "temp.h"
#include <math.h>
#include <stdio.h>

float adjustedTime(EdgeAttr attr, TimeContext ctx)
{
    float time = attr.baseTime;
    if ((ctx.hour >= 7 && ctx.hour <= 9) || (ctx.hour >= 17 && ctx.hour <= 19))
    {
        time *= 1.5f;
    }
    if (attr.roadType == 1 && ctx.season == 1)
    {
        return -1.0f; // route de latérite inaccessible
    }
    return time;
}