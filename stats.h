#ifndef STATS_H
#define STATS_H

#include "simulator.h"

typedef struct {
    double avg_temp;
    double avg_humid;
    double max_temp;
    char max_temp_ts[32];
    double max_humid;
    char max_humid_ts[32];
} Statistics;

void calc_statistics(const Reading *readings, int n, Statistics *stats);

#endif