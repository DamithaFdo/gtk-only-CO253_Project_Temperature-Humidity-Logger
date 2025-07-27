# Test Plan - Temperature & Humidity Logger

## Overview
This document outlines comprehensive testing procedures for the Temperature & Humidity Logger application, including unit tests, integration tests, and user acceptance tests.

## Test Environment Setup

### Prerequisites
- GTK3 development libraries installed
- GCC compiler available
- pkg-config configured for GTK3
- Windows/Linux environment with terminal access

### Test Data Files
- `readings.csv` - Default simulation data
- `test_readings.csv` - Generated from test.c
- Custom CSV files for boundary testing

## 1. Unit Tests

### 1.1 Statistics Module (`stats.c`)

#### Test Case 1.1.1: Basic Statistics Calculation
**Function**: `calc_statistics()`

**Input**:
```c
Reading test_readings[3] = {
    {1, "2025-01-01 00:00", 25.0, 55.0},
    {2, "2025-01-01 00:30", 30.0, 65.0},
    {3, "2025-01-01 01:00", 20.0, 60.0}
};
```

**Expected Output**:
```c
Statistics expected = {
    .avg_temp = 25.0,        // (25+30+20)/3
    .avg_humid = 60.0,       // (55+65+60)/3
    .max_temp = 30.0,
    .max_temp_ts = "2025-01-01 00:30",
    .max_humid = 65.0,
    .max_humid_ts = "2025-01-01 00:30"
};
```

#### Test Case 1.1.2: Single Reading
**Input**:
```c
Reading single_reading[1] = {
    {1, "2025-01-01 12:00", 22.5, 58.3}
};
```

**Expected Output**:
```c
Statistics expected = {
    .avg_temp = 22.5,
    .avg_humid = 58.3,
    .max_temp = 22.5,
    .max_temp_ts = "2025-01-01 12:00",
    .max_humid = 58.3,
    .max_humid_ts = "2025-01-01 12:00"
};
```

#### Test Case 1.1.3: Identical Values
**Input**:
```c
Reading identical_readings[4] = {
    {1, "2025-01-01 00:00", 25.0, 50.0},
    {2, "2025-01-01 00:30", 25.0, 50.0},
    {3, "2025-01-01 01:00", 25.0, 50.0},
    {4, "2025-01-01 01:30", 25.0, 50.0}
};
```

**Expected Output**:
```c
Statistics expected = {
    .avg_temp = 25.0,
    .avg_humid = 50.0,
    .max_temp = 25.0,
    .max_temp_ts = "2025-01-01 00:00",  // First occurrence
    .max_humid = 50.0,
    .max_humid_ts = "2025-01-01 00:00"  // First occurrence
};
```

### 1.2 Simulator Module (`simulator.c`)

#### Test Case 1.2.1: Random Data Generation
**Function**: `generate_random_readings()`

**Input**:
```c
Reading readings[5];
generate_random_readings(readings, 5);
```

**Expected Output Validation**:
- All `seq_no` values: 1, 2, 3, 4, 5
- Temperature range: 50.0 ≤ temp ≤ 79.0
- Humidity range: 50.0 ≤ humidity ≤ 79.0
- Timestamps: Sequential with 28.8-minute intervals
- First timestamp: Today at 00:00:00

#### Test Case 1.2.2: CSV File Reading - Valid Format
**Function**: `read_csv_readings()`

**Input File** (`valid_test.csv`):
```
1,2025-01-01 08:00,23.5,45.2
2,2025-01-01 08:30,24.1,46.8
3,2025-01-01 09:00,22.9,44.5
```

**Expected Output**:
- Return value: `true`
- readings[0]: {1, "2025-01-01 08:00", 23.5, 45.2}
- readings[1]: {2, "2025-01-01 08:30", 24.1, 46.8}
- readings[2]: {3, "2025-01-01 09:00", 22.9, 44.5}

#### Test Case 1.2.3: CSV File Reading - Invalid Format
**Function**: `read_csv_readings()`

**Input File** (`invalid_test.csv`):
```
1,2025-01-01 08:00,23.5
2,invalid,24.1,46.8
3,2025-01-01 09:00,22.9,44.5
```

**Expected Output**:
- Return value: `false`
- Function should handle malformed lines gracefully

#### Test Case 1.2.4: Non-existent File
**Function**: `read_csv_readings()`

**Input**: `"nonexistent.csv"`

**Expected Output**:
- Return value: `false`
- No crash or memory corruption

## 2. Integration Tests

### 2.1 Main Application GUI Tests

#### Test Case 2.1.1: Random Data Mode
**Steps**:
1. Compile and run main application
2. Select "Random Simulation" mode
3. Set readings count to 10
4. Click "Generate Data"

**Expected Output**:
- Table displays 10 rows of data
- Temperature/humidity values in expected ranges
- Status colors: Green/Yellow/Red based on thresholds
- Statistics panel shows calculated averages and maximums

#### Test Case 2.1.2: CSV File Input Mode
**Steps**:
1. Ensure `readings.csv` exists with valid data
2. Select "CSV File Input" mode
3. Click "Load Data"

**Expected Output**:
- Data loaded from CSV file displayed in table
- Correct parsing of all fields
- Statistics calculated and displayed
- Status indicators match threshold rules

#### Test Case 2.1.3: Test CSV Input Mode
**Steps**:
1. Run `test.c` to generate `test_readings.csv`
2. Select "Test CSV Input" mode in main app
3. Click "Load Data"

**Expected Output**:
- Manual test data displayed correctly
- Statistics match those calculated in test.c
- Consistent status indicators

### 2.2 Threshold Testing

#### Test Case 2.2.1: Alert Threshold Validation
**Input Data**:
```csv
1,2025-01-01 08:00,66.0,66.0
2,2025-01-01 08:30,70.0,70.0
3,2025-01-01 09:00,65.1,65.1
```

**Expected Output**:
- All rows display RED status indicators
- Status text contains "Alert! Safe Temperature exceeded."
- Status text contains "Alert! Safe Humidity exceeded."

#### Test Case 2.2.2: Warning Threshold Validation
**Input Data**:
```csv
1,2025-01-01 08:00,60.0,60.0
2,2025-01-01 08:30,62.5,62.5
3,2025-01-01 09:00,64.9,64.9
```

**Expected Output**:
- All rows display YELLOW status indicators
- Status text contains "Warning! Safe Temperature exceeding."
- Status text contains "Warning! Safe Humidity exceeding."

#### Test Case 2.2.3: Safe Level Validation
**Input Data**:
```csv
1,2025-01-01 08:00,55.0,55.0
2,2025-01-01 08:30,58.5,58.5
3,2025-01-01 09:00,59.9,59.9
```

**Expected Output**:
- All rows display GREEN status indicators
- Status text contains "Safe Temperature levels."
- Status text contains "Safe Humidity levels."

## 3. Test Program Validation (`test.c`)

### 3.1 Manual Input Testing

#### Test Case 3.1.1: Valid Numeric Input
**Manual Input Sequence**:
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

**Expected Console Output**:
```
****************************************
  READINGS TABLE
****************************************
Seq | Timestamp           | Temp(C) | Humid(%) | Temp Status Expected           | Humid Status Expected
----+---------------------+---------+----------+-------------------------------+------------------------------
1   | 2025-07-27 HH:MM:SS | 25.50   | 55.00    | Safe Temperature levels.       | Safe Humidity levels.
2   | 2025-07-27 HH:MM:SS | 67.00   | 62.00    | Alert! Safe Temperature exceeded. | Warning! Safe Humidity exceeding.
3   | 2025-07-27 HH:MM:SS | 58.00   | 48.00    | Safe Temperature levels.       | Safe Humidity levels.
4   | 2025-07-27 HH:MM:SS | 61.50   | 70.00    | Warning! Safe Temperature exceeding. | Alert! Safe Humidity exceeded.
5   | 2025-07-27 HH:MM:SS | 29.00   | 52.00    | Safe Temperature levels.       | Safe Humidity levels.

****************************************
  CALCULATED STATISTICS
****************************************
Total Readings: 5
Average Temp: 48.20 deg C
Average Humid: 57.40 %
Max Temp: 67.00 deg C at 2025-07-27 HH:MM:SS
Max Humid: 70.00 % at 2025-07-27 HH:MM:SS
```

**Expected CSV Output** (`test_readings.csv`):
```
1,2025-07-27 HH:MM:SS,25.50,55.00
2,2025-07-27 HH:MM:SS,67.00,62.00
3,2025-07-27 HH:MM:SS,58.00,48.00
4,2025-07-27 HH:MM:SS,61.50,70.00
5,2025-07-27 HH:MM:SS,29.00,52.00
```

#### Test Case 3.1.2: Invalid Input Handling
**Manual Input Sequence**:
```
Reading 1 Temperature: abc
(Expected: "Invalid input! Please enter a number.")
Reading 1 Temperature: 25.5
Reading 1 Humidity: xyz
(Expected: "Invalid input! Please enter a number.")
Reading 1 Humidity: 55.0
```

**Expected Behavior**:
- Error message displayed for invalid input
- Program re-prompts for the same reading
- No advancement to next reading until valid input

## 4. Boundary Testing

### 4.1 Extreme Values

#### Test Case 4.1.1: Very Large Numbers
**Input**: Temperature: 999.99, Humidity: 999.99
**Expected**: Program handles without overflow/crash

#### Test Case 4.1.2: Negative Values
**Input**: Temperature: -50.0, Humidity: -10.0
**Expected**: Program processes but may show unexpected status

#### Test Case 4.1.3: Zero Values
**Input**: Temperature: 0.0, Humidity: 0.0
**Expected**: Safe status (below warning threshold)

### 4.2 Large Dataset Testing

#### Test Case 4.2.1: Maximum Readings (50)
**Input**: Generate 50 random readings
**Expected Output**:
- All 50 readings displayed in GUI
- Statistics calculated correctly
- No memory leaks or performance issues
- Scroll functionality works properly

#### Test Case 4.2.2: Empty Dataset
**Input**: 0 readings
**Expected**: Graceful handling, empty table display

## 5. Error Handling Tests

### 5.1 File System Errors

#### Test Case 5.1.1: Read-Only CSV File
**Setup**: Make test CSV file read-only
**Expected**: Error message, graceful degradation

#### Test Case 5.1.2: Insufficient Disk Space
**Setup**: Fill disk space before CSV generation
**Expected**: Error message in test.c, no crash

#### Test Case 5.1.3: Corrupted CSV File
**Input File**:
```
1,2025-01-01 08:00,23.5,45.2
@#$%^&*()invalid_line
3,2025-01-01 09:00,22.9,44.5
```
**Expected**: Skip invalid lines, process valid ones

## 6. Performance Tests

### 6.1 Response Time Testing
- Data generation: < 1 second for 50 readings
- CSV loading: < 2 seconds for typical file sizes
- Statistics calculation: < 100ms for 50 readings
- GUI responsiveness: No freezing during operations

### 6.2 Memory Usage
- No memory leaks during repeated operations
- Stable memory usage with large datasets
- Proper cleanup on application exit

## 7. User Acceptance Tests

### 7.1 Workflow Testing

#### Test Case 7.1.1: Complete End-to-End Workflow
**Steps**:
1. Run test.c with sample data
2. Launch main application
3. Load test CSV data
4. Verify status indicators
5. Check statistics accuracy
6. Generate new random data
7. Compare with test data

**Success Criteria**:
- All operations complete without errors
- Data consistency across modules
- Intuitive user interface
- Accurate calculations

### 7.2 Usability Testing
- Interface elements clearly labeled
- Error messages are informative
- Recovery from errors is possible
- Help/documentation is accessible

## 8. Test Automation Scripts

### 8.1 Compilation Test Script
```bash
#!/bin/bash
echo "Testing compilation..."
gcc test.c -o test.exe && echo "test.c: PASS" || echo "test.c: FAIL"
gcc main.c simulator.c stats.c -o app $(pkg-config --cflags --libs gtk+-3.0) && echo "main app: PASS" || echo "main app: FAIL"
```

### 8.2 Data Validation Script
```bash
#!/bin/bash
./test.exe < input_data.txt
if [ -f "test_readings.csv" ]; then
    echo "CSV generation: PASS"
    # Validate CSV format
    if grep -q "^[0-9]\+,[0-9-: ]\+,[0-9.]\+,[0-9.]\+$" test_readings.csv; then
        echo "CSV format: PASS"
    else
        echo "CSV format: FAIL"
    fi
else
    echo "CSV generation: FAIL"
fi
```

## 9. Test Results Documentation

### 9.1 Test Execution Log Template
```
Test Date: YYYY-MM-DD
Tester: [Name]
Environment: [OS/Compiler versions]

Test Case ID | Description | Status | Notes
-------------|-------------|--------|-------
1.1.1       | Basic Stats | PASS   | All calculations correct
1.1.2       | Single Read | PASS   | Handles edge case
[...]
```

### 9.2 Bug Report Template
```
Bug ID: [Unique identifier]
Severity: [Critical/High/Medium/Low]
Test Case: [Related test case]
Description: [What went wrong]
Steps to Reproduce: [Detailed steps]
Expected Result: [What should happen]
Actual Result: [What actually happened]
Environment: [System details]
Status: [Open/Fixed/Closed]
```

## 10. Regression Testing

### 10.1 Smoke Tests
After any code changes, run these critical tests:
1. Application compiles without errors
2. Basic data loading works
3. Statistics calculation is accurate
4. GUI displays properly
5. No crashes during normal operation

### 10.2 Full Test Suite
- Run complete test suite before releases
- Verify all test cases pass
- Document any new issues
- Update test cases for new features

## Conclusion
This test plan ensures comprehensive coverage of all application components, from individual function testing to complete user workflows. Regular execution of these tests will maintain software quality and reliability.
