# Test Plan - Temperature & Humidity Logger
## Designed by E/21/132 E/21/139

**My Repository**: 
```bash
https://github.com/DamithaFdo/gtk-only-CO253_Project_Temperature-Humidity-Logger.git
```

## Overview

This test plan focuses on the independent testing utility (`test.c`) that allows manual data entry, processes readings through simple loop-based algorithms, calculates statistics, and generates CSV files for validation in the main GUI application (`app.exe`).

## Test Environment Setup

### Prerequisites
- GTK3 development libraries installed
- GCC compiler available
- pkg-config configured for GTK3
- Windows/Linux environment with MSYS2 MinGW terminal access

### Test Data Files
- `test_readings.csv` - Generated from test.exe for main application validation

## Testing Methodology

The testing follows this workflow:
1. **Independent Testing**: Run `test.c` as a standalone program
2. **Manual Input**: Enter 5 sets of temperature/humidity readings
3. **Processing**: Calculate statistics using simple loops 
4. **CSV Generation**: Create `test_readings.csv` file
5. **Validation**: Import CSV into `app.exe` and compare results

## Independent Test Program (`test.c`)

### Core Functions

### `get_status()` Function
 Determines status based on threshold values

**Expected Outputs**:
- Alert: `"Alert! Safe Temperature exceeded."` / `"Alert! Safe Humidity exceeded."`
- Warning: `"Warning! Safe Temperature exceeding."` / `"Warning! Safe Humidity exceeding."`
- Safe: `"Safe Temperature levels."` / `"Safe Humidity levels."`

### `generate_timestamp()` Function
Creates dummy timestamps with minute increments

### `main()` Function - Complete Workflow
Manage the entire testing process from input to output

**Main Function Flow**:
```c
int main() {
    // 1. Collect 5 readings from user via get_user_readings()
    // 2. Calculate and display statistic
    // 3. Display formatted readings table with status
    // 4. Generate CSV file
    // 5. Display success message and wait for user
}
```

**Detailed Workflow**

1. **Initialization**
   ```
   ****************************************
     MANUAL TEMPERATURE & HUMIDITY TEST
   ****************************************
   ```
   - Declares Reading array for 5 elements and setup data structure

2. **Data Collection** (via `get_user_readings()`)
   - Prompts for temperature and humidity for each of 5 readings
   - Generates timestamps using `generate_timestamp()`

3. **Data Processing and Display**
   - Creates formatted table showing all readings
   - Applies threshold logic via `get_status()` for each value

4. **Statistical Analysis**
   ```c
   // Simple loop calculations
   double sum_temp = 0, sum_humid = 0;
   double max_temp = readings[0].temperature;
   double max_humid = readings[0].humidity;
   
   for (int i = 0; i < TEST_READINGS_COUNT; i++) {
       sum_temp += readings[i].temperature;
       sum_humid += readings[i].humidity;
       // Find maximums with timestamps
   }
   ```

5. **File Generation**
   - Calls `create_test_csv()` to generate output file
   - Provides success/failure feedback

6. **Completion**
   - Displays final success message
   - Waits for user input before termination

### `create_test_csv()` Function  
Writes readings data to CSV file

**Expected CSV Output** (`test_readings.csv`)
```
1,2025-07-27 HH:MM:SS,25.50,55.00
2,2025-07-27 HH:MM:SS,67.00,62.00
3,2025-07-27 HH:MM:SS,58.00,48.00
4,2025-07-27 HH:MM:SS,61.50,70.00
5,2025-07-27 HH:MM:SS,29.00,52.00
```

### `get_user_readings()` Function
Manually taking user data inputs and prepares struc


## Independent Test Program (`test.exe`)

**Test Procedure**
1. Compile and run: `gcc test.c -o test.exe && ./test.exe`
2. Enter the following test data when prompted;

**Sample Input Data**
```
Reading 1 Temperature: 25.5
Reading 1 Humidity: 55.0
Reading 2 Temperature: 67.0  
Reading 2 Humidity: 62.0
Reading 3 Temperature: 58.0
Reading 3 Humidity: 48.0
Reading 4 Temperature: 61.5
Reading 4 Humidity: 70.0
Reading 5 Temperature: 29.0
Reading 5 Humidity: 52.0
```

**Manual Calculation**
- Sum Temperature: 25.5 + 67.0 + 58.0 + 61.5 + 29.0 = 241.0
- Sum Humidity: 55.0 + 62.0 + 48.0 + 70.0 + 52.0 = 287.0
- Average Temperature: 241.0 ÷ 5 = 48.20°C
- Average Humidity: 287.0 ÷ 5 = 57.40%
- Max Temperature: 67.0°C (Reading 2)
- Max Humidity: 70.0% (Reading 4)

**Expected Console Output**
```
****************************************
  CALCULATED STATISTICS
****************************************
Total Readings: 5
Average Temp: 48.20 deg C
Average Humid: 57.40 %
Max Temp: 67.00 deg C at [timestamp of reading 2]
Max Humid: 70.00 % at [timestamp of reading 4]
```

**Expected Tabular output**
```
****************************************
  READINGS TABLE
****************************************
Seq | Timestamp           | Temp(C) | Humid(%) | Temp Status Expected                 | Humid Status Expected
----+---------------------+---------+----------+--------------------------------------+----------------------------------
1   | 20YY-MM-DD HH:MM:SS | 25.50   | 55.00    | Safe Temperature levels.             | Safe Humidity levels.
2   | 20YY-MM-DD HH:MM:SS | 67.00   | 62.00    | Alert! Safe Temperature exceeded.    | Warning! Safe Humidity exceeding.
3   | 20YY-MM-DD HH:MM:SS | 58.00   | 48.00    | Safe Temperature levels.             | Safe Humidity levels.
4   | 20YY-MM-DD HH:MM:SS | 61.50   | 70.00    | Warning! Safe Temperature exceeding. | Alert! Safe Humidity exceeded.
5   | 20YY-MM-DD HH:MM:SS | 29.00   | 52.00    | Safe Temperature levels.             | Safe Humidity levels.
```

**Expected CSV Output** (`test_readings.csv`):
```
1,20YY-MM-DD HH:MM:SS,25.50,55.00
2,20YY-MM-DD HH:MM:SS,67.00,62.00
3,20YY-MM-DD HH:MM:SS,58.00,48.00
4,20YY-MM-DD HH:MM:SS,61.50,70.00
5,20YY-MM-DD HH:MM:SS,29.00,52.00
```
### Wait for termination

## Independent Test Program (`app.exe`)

**Test Procedure**:
1. Compile and run using MSYS2 MinGW Terminal: 
   ```bash
   gcc main.c simulator.c stats.c -o app $(pkg-config --cflags --libs gtk+-3.0) && ./app.exe
   ```
2. Select Input mode: Test CSV file
3. Start simulation process  
4. Compare actual results with expected outputs
5. Verify GUI functionality and data display accuracy