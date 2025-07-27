// simulator.c - Temperature and Humidity Data Simulation Module

#include "simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define GAP_MINUTES 28.8

// Cross-platform function to start time with 0000hrs timestamp
static time_t get_today_midnight() {
    time_t now = time(NULL);
    struct tm tm_info;
    localtime_s(&tm_info, &now);
    tm_info.tm_hour = 0;
    tm_info.tm_min = 0;
    tm_info.tm_sec = 0;
    return mktime(&tm_info);  // Convert back to time_t
}


// Generates temperature and humidity readings for simulation 
void generate_random_readings(Reading *readings, int n) {
    time_t t = get_today_midnight(); // Start timestamps at 0000hrs
    srand((unsigned)time(NULL));     // Seed random generator for different values each run
    
    for (int i = 0; i < n; i++) {
        readings[i].seq_no = i + 1;  // Sequential numbering
        
        // Calculate timestamp - keeps 28.8 minutes gap
        time_t ts = t + (int)(i * GAP_MINUTES * 60);
        strftime(readings[i].timestamp, 32, "%Y-%m-%d %H:%M", localtime(&ts));
        
        // Generate temp humid values
        readings[i].temperature = 50.0 + rand() % 30; // Temperature 50-79
        readings[i].humidity = 50.0 + rand() % 30;    // Humidity 50-79%
    }
}

bool read_csv_readings(const char *filename, Reading *readings, int n) {
    FILE *f = fopen(filename, "r");
    if (!f) return false;  // Return false if file cannot be opened
    
    char line[128];  // Buffer for reading each line
    int i = 0;       // Counter for successfully read readings
    
    // Read file line by line until end
    while (fgets(line, sizeof(line), f) && i < n) {
        int seq;
        char ts[32];
        double temp, humid;

        // Parse seq_no,timestamp,temperature,humidity
        if (sscanf(line, "%d,%31[^,],%lf,%lf", &seq, ts, &temp, &humid) == 4) {
            //If all 4 fields are successfully parsed
            readings[i].seq_no = seq;
            strncpy(readings[i].timestamp, ts, 32);
            readings[i].timestamp[31] = '\0';
            readings[i].temperature = temp;
            readings[i].humidity = humid;
            i++;
        }
        // Skip lines that incomplete
    }
    
    fclose(f);
    return (i == n);  // Return true only if we got exactly the expected number of readings
}