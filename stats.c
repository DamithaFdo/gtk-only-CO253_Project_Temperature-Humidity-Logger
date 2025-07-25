#include "stats.h"
#include <string.h>

void calc_statistics(const Reading *readings, int n, Statistics *stats) {
    double sum_temp = 0, sum_humid = 0;
    stats->max_temp = readings[0].temperature;
    strcpy(stats->max_temp_ts, readings[0].timestamp);
    stats->max_humid = readings[0].humidity;
    strcpy(stats->max_humid_ts, readings[0].timestamp);

    for (int i = 0; i < n; i++) {
        sum_temp += readings[i].temperature;
        sum_humid += readings[i].humidity;
        if (readings[i].temperature > stats->max_temp) {
            stats->max_temp = readings[i].temperature;
            strcpy(stats->max_temp_ts, readings[i].timestamp);
        }
        if (readings[i].humidity > stats->max_humid) {
            stats->max_humid = readings[i].humidity;
            strcpy(stats->max_humid_ts, readings[i].timestamp);
        }
    }
    stats->avg_temp = sum_temp / n;
    stats->avg_humid = sum_humid / n;
}