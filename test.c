#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stats.h"
#include "simulator.h"

//gcc test.c stats.c simulator.c -o test.exe

#define ALERT_THRESHOLD 65.0
#define WARNING_THRESHOLD 60.0
#define TEST_READINGS_COUNT 5

// Function to get temperature and humidity status
void get_status(double value, char* status, const char* type) {
    if (value > ALERT_THRESHOLD) {
        sprintf(status, "Alert! Safe %s exceeded.", type);
    } else if (value >= WARNING_THRESHOLD) {
        sprintf(status, "Warning! Safe %s exceeding.", type);
    } else {
        sprintf(status, "Safe %s levels.", type);
    }
}

// Function to generate dummy timestamp
void generate_timestamp(char* timestamp, int seq) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    // Add seq minutes to create different timestamps
    tm_info->tm_min += seq;
    mktime(tm_info); // Normalize the time structure
    strftime(timestamp, 32, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Function to get user input for readings
void get_user_readings(Reading readings[TEST_READINGS_COUNT]) {
    printf("========================================\n");
    printf("  MANUAL DATA ENTRY MODE\n");
    printf("========================================\n");
    printf("Please enter %d temperature and humidity readings:\n\n", TEST_READINGS_COUNT);
    
    for (int i = 0; i < TEST_READINGS_COUNT; i++) {
        readings[i].seq_no = i + 1;
        
        printf("Reading %d:\n", i + 1);
        printf("Enter Seq %d Temperature (deg C): ", i + 1);
        if (scanf("%lf", &readings[i].temperature) != 1) {
            printf("Invalid input! Please enter a number.\n");
            i--; // Retry this reading
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        
        printf("Enter Seq %d Humidity (%%): ", i + 1);
        if (scanf("%lf", &readings[i].humidity) != 1) {
            printf("Invalid input! Please enter a number.\n");
            i--; // Retry this reading
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        
        // Generate timestamp
        generate_timestamp(readings[i].timestamp, i);
        
        printf("Reading %d recorded: %.2f deg C, %.2f%%\n\n", i + 1, 
               readings[i].temperature, readings[i].humidity);
    }
}

// Function to create CSV file with readings
int create_test_csv(Reading readings[TEST_READINGS_COUNT], const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not create file '%s'\n", filename);
        return 0;
    }
    
    // Write data rows (no header needed)
    for (int i = 0; i < TEST_READINGS_COUNT; i++) {
        fprintf(file, "%d,%s,%.2f,%.2f\n",
                readings[i].seq_no,
                readings[i].timestamp,
                readings[i].temperature,
                readings[i].humidity);
    }
    
    fclose(file);
    printf("CSV file '%s' created successfully!\n\n", filename);
    return 1;
}

// Function to display readings in table format
void display_readings_table(Reading readings[TEST_READINGS_COUNT]) {
    printf("========================================\n");
    printf("  READINGS TABLE\n");
    printf("========================================\n");
    printf("%-4s | %-19s | %-8s | %-9s | %-35s | %-35s\n", 
           "Seq", "Timestamp", "Temp(C)", "Humid(%)", "Temp Status Expected", "Humid Status Expected");
    printf("-----+---------------------+----------+-----------+-------------------------------------+-------------------------------------\n");
    
    for (int i = 0; i < TEST_READINGS_COUNT; i++) {
        char temp_status[50], humid_status[50];
        get_status(readings[i].temperature, temp_status, "Temperature");
        get_status(readings[i].humidity, humid_status, "Humidity");
        
        printf("%-4d | %-19s | %-8.2f | %-9.2f | %-35s | %-35s\n",
               readings[i].seq_no,
               readings[i].timestamp,
               readings[i].temperature,
               readings[i].humidity,
               temp_status,
               humid_status);
    }
    printf("\n");
}

int main() {
    printf("========================================\n");
    printf("  MANUAL TEMPERATURE & HUMIDITY TEST\n");
    printf("========================================\n\n");
    
    Reading readings[TEST_READINGS_COUNT];
    Statistics stats;
    
    // Get user input for readings
    get_user_readings(readings);
    
    // Display readings in table format
    display_readings_table(readings);
    
    // Calculate statistics
    calc_statistics(readings, TEST_READINGS_COUNT, &stats);
    
    // Display statistics (same format as GUI)
    printf("========================================\n");
    printf("  CALCULATED STATISTICS\n");
    printf("========================================\n");
    printf("Total Readings: %d\n", TEST_READINGS_COUNT);
    printf("Average Temp: %.2f deg C\n", stats.avg_temp);
    printf("Average Humid: %.2f %%\n", stats.avg_humid);
    printf("Max Temp: %.2f deg C at %s\n", stats.max_temp, stats.max_temp_ts);
    printf("Max Humid: %.2f %% at %s\n", stats.max_humid, stats.max_humid_ts);
    printf("\n");
    
    // Creates test_readings.csv file
    if (create_test_csv(readings, "test_readings.csv")) {
        printf("========================================\n");
        printf("  SUCCESS!\n");
        printf("========================================\n");
        printf("Test data saved to 'test_readings.csv'\n");
        printf("Select 'CSV File Input' mode in GUI to verify Operation.\n");
        printf("========================================\n");
    } else {
        printf("Error creating CSV file!\n");
        
        printf("\nPress any key to continue...");
        getchar(); // Clear any remaining input
        getchar(); // Wait for user input
        return 1;
    }
    
    printf("\nPress any key to continue...");
    getchar(); // Clear any remaining input  
    getchar(); // Wait for user input
    
    return 0;
}