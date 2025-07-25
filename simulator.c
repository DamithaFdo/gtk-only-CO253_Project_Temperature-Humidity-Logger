#include "simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> // <-- THIS LINE FIXES THE ERROR

#define GAP_MINUTES 28.8

void generate_random_readings(Reading *readings, int n) {
    time_t t = time(NULL);
    for (int i = 0; i < n; i++) {
        readings[i].seq_no = i + 1;
        time_t ts = t + (int)(i * GAP_MINUTES * 60);
        strftime(readings[i].timestamp, 32, "%Y-%m-%d %H:%M", localtime(&ts));
        readings[i].temperature = 50.0 + rand() % 30; // 50-79
        readings[i].humidity = 50.0 + rand() % 30; // 50-79
    }
}

bool read_csv_readings(const char *filename, Reading *readings, int n) {
    FILE *f = fopen(filename, "r");
    if (!f) return false;
    char line[128];
    int i = 0;
    while (fgets(line, sizeof(line), f) && i < n) {
        int seq;
        char ts[32];
        double temp, humid;
        if (sscanf(line, "%d,%31[^,],%lf,%lf", &seq, ts, &temp, &humid) == 4) {
            readings[i].seq_no = seq;
            strncpy(readings[i].timestamp, ts, 32);
            readings[i].timestamp[31] = '\0'; // Ensure null-termination
            readings[i].temperature = temp;
            readings[i].humidity = humid;
            i++;
        }
    }
    fclose(f);
    return (i == n);
}