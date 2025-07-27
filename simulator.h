// simulator.h - Declaration of simulator.c functions and structures

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdbool.h>

// Declaration of structure to store temperature and humidity readings
typedef struct {
    int seq_no;
    char timestamp[32];
    double temperature;
    double humidity;
} Reading;

// Declaration of required functions
void generate_random_readings(Reading *readings, int n);
bool read_csv_readings(const char *filename, Reading *readings, int n);

#endif