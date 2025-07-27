// stats.h - Declaration of stats.c functions and structures

#ifndef STATS_H
#define STATS_H

#include "simulator.h"

// Declaration of structure to store stat values
typedef struct {
    double avg_temp;        
    double avg_humid;       
    double max_temp;        
    char max_temp_ts[32];   
    double max_humid;       
    char max_humid_ts[32]; 
} Statistics;

// Declaration of main stat function
void calc_statistics(const Reading *readings, int n, Statistics *stats);

#endif