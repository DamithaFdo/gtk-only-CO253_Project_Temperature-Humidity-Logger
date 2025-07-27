#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//gcc test.c -o test.exe

#define ALERT_THRESHOLD 65.0
#define WARNING_THRESHOLD 60.0
#define TEST_READINGS_COUNT 5

typedef struct {
    int seq_no;
    char timestamp[32];
    double temperature;
    double humidity;
} Reading;

// Simple function to determine status using conditional logic
void get_status(double value, char* status, const char* type) {
    if (value > ALERT_THRESHOLD) {
        sprintf(status, "Alert! Safe %s exceeded.", type);
    } else if (value >= WARNING_THRESHOLD) {
        sprintf(status, "Warning! Safe %s exceeding.", type);
    } else {
        sprintf(status, "Safe %s levels.", type);
    }
}

//Dummy timestamp generation
void generate_timestamp(char* timestamp, int seq) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    tm_info->tm_min += seq;
    mktime(tm_info);
    strftime(timestamp, 32, "%Y-%m-%d %H:%M:%S", tm_info);
}

void get_user_readings(Reading readings[TEST_READINGS_COUNT]) {
    printf("****************************************\n");
    printf("  MANUAL DATA ENTRY MODE\n");
    printf("****************************************\n");
    printf("Please enter %d temperature and humidity readings:\n\n", TEST_READINGS_COUNT);
    
    for (int i = 0; i < TEST_READINGS_COUNT; i++) {
        readings[i].seq_no = i + 1;
        
        printf("Reading %d:\n", i + 1);
        
        printf("Enter Seq %d Temperature (deg C): ", i + 1);
        if (scanf("%lf", &readings[i].temperature) != 1) {
            printf("Invalid input! Please enter a number.\n");
            i--;
            while (getchar() != '\n');
            continue;
        }
        printf("Enter Seq %d Humidity (%%): ", i + 1);
        if (scanf("%lf", &readings[i].humidity) != 1) {
            printf("Invalid input! Please enter a number.\n");
            i--;
            while (getchar() != '\n');
            continue;
        }
    
        generate_timestamp(readings[i].timestamp, i);
        
        printf("Reading %d recorded: %.2f deg C, %.2f%%\n\n", i + 1, 
               readings[i].temperature, readings[i].humidity);
    }
}

int create_test_csv(Reading readings[TEST_READINGS_COUNT], const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not create file '%s'\n", filename);
        return 0;
    }

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

int main() {
    printf("****************************************\n");
    printf("  MANUAL TEMPERATURE & HUMIDITY TEST\n");
    printf("****************************************\n\n");
    
    Reading readings[TEST_READINGS_COUNT];

    get_user_readings(readings);
    
    printf("****************************************\n");
    printf("  READINGS TABLE\n");
    printf("****************************************\n");
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
    
    double sum_temp = 0, sum_humid = 0;
    double max_temp = readings[0].temperature;
    char max_temp_time[32];
    strcpy(max_temp_time, readings[0].timestamp);
    double max_humid = readings[0].humidity;
    char max_humid_time[32];
    strcpy(max_humid_time, readings[0].timestamp);
    
    // Simple loop to calculate totals and find maximums
    for (int i = 0; i < TEST_READINGS_COUNT; i++) {
        sum_temp += readings[i].temperature;
        sum_humid += readings[i].humidity;
        
        if (readings[i].temperature > max_temp) {
            max_temp = readings[i].temperature;
            strcpy(max_temp_time, readings[i].timestamp);
        }
        
        if (readings[i].humidity > max_humid) {
            max_humid = readings[i].humidity;
            strcpy(max_humid_time, readings[i].timestamp);
        }
    }
    
    double avg_temp = sum_temp / TEST_READINGS_COUNT;
    double avg_humid = sum_humid / TEST_READINGS_COUNT;
    
    // Display statistics
    printf("****************************************\n");
    printf("  CALCULATED STATISTICS\n");
    printf("****************************************\n");
    printf("Total Readings: %d\n", TEST_READINGS_COUNT);
    printf("Average Temp: %.2f deg C\n", avg_temp);
    printf("Average Humid: %.2f %%\n", avg_humid);
    printf("Max Temp: %.2f deg C at %s\n", max_temp, max_temp_time);
    printf("Max Humid: %.2f %% at %s\n", max_humid, max_humid_time);
    printf("\n");
    
    if (create_test_csv(readings, "test_readings.csv")) {
        printf("****************************************\n");
        printf("  SUCCESS!\n");
        printf("****************************************\n");
        printf("Test data saved to 'test_readings.csv'\n");
        printf("Select 'CSV File Input' mode in GUI to verify Operation.\n");
        printf("****************************************\n");
    } else {
        printf("Error creating CSV file!\n");
        printf("\nPress any key to continue...");
        getchar();
        getchar();
        return 1;
    }
    
    // Wait for user input to close console
    printf("\nPress any key to continue...");
    getchar();
    getchar();
    
    return 0;
}