#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdbool.h>

typedef struct {
    int seq_no;
    char timestamp[32];
    double temperature;
    double humidity;
} Reading;

void generate_random_readings(Reading *readings, int n);
bool read_csv_readings(const char *filename, Reading *readings, int n);

#endif