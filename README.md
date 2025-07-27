# Temperature & Humidity Logger BY Group 08
## Designed by E/21/132 E/21/139

This application focuses on creating a software-only simulated application for processing temperature and humidity data received from sensor platforms. This development was done for focusing on greenhouse condition monitoring.
We used the GTK library for creating the GUI.


## Project Objectives

This project implements a comprehensive temperature and humidity monitoring system with the following objectives:
- Real-time data processing (simulation mode)
- Alert/Warning threshold monitoring
- Create data logging for further use
- Functionality testing to maintain reliability

## File Structure and Functions

### Header Files

### `simulator.h`
**Purpose**: Declares data simulation and CSV reading functions

**Structures**:
- `Reading`: Core data structure for temperature/humidity readings
  - `seq_no`: Sequence number of reading
  - `timestamp[32]`: Date and time string
  - `temperature`: Temperature value in Celcius 
  - `humidity`: Relative Humidity (as a percentage)

**Function Declarations**:
- `void generate_random_readings(Reading *readings, int n)`: Generates simulated sensor data
- `bool read_csv_readings(const char *filename, Reading *readings, int n)`: Reads data from CSV files

### `stats.h`
**Purpose**: Declares statistics calculation functions and required data structures

**Structures**:
- `Statistics`: Stores calculated statistical values
  - `avg_temp`: Average temperature
  - `avg_humid`: Average humidity  
  - `max_temp`: Maximum temperature
  - `max_temp_ts[32]`: Timestamp of maximum temperature
  - `max_humid`: Maximum humidity
  - `max_humid_ts[32]`: Timestamp of maximum humidity

**Function Declarations**:
- `void calc_statistics(const Reading *readings, int n, Statistics *stats)`: Main statistics calculation function

### Implementation Files

### `stats.c`
**Purpose**: Statistics calculation module

**Implemented Functions**:
- `calc_statistics()`: Calculates:
  - Average temperature and humidity
  - Maximum temperature and humidity values
  - Timestamps of maximum readings

### `simulator.c`
**Purpose**: Data simulation and CSV file handling module

**Internal Functions**:
- `get_today_midnight()`: Cross-platform function for timestamp generation starting at 00:00

**Implemented Functions**:
- `generate_random_readings()`: 
  - Creates temperature in ranges (50-79°C) and humidity (50-79%)
  - Generates timestamps with 28.8-minute intervals starting from 00:00hrs
  
- `read_csv_readings()`:
  - Parses CSV files with format: `seq_no,timestamp,temperature,humidity`
  - Returns success/failure status
  - Verifies expected number of readings

### `main.c`
**Purpose**: Main application with GUI (using GTK3 library)

**Features**:
- Multiple data input modes:
  1. Random simulation
  2. Default CSV file (`readings.csv`)
  3. Test CSV file (`test_readings.csv`)
- Real-time status monitoring with color coding:
  - **Green**: Safe levels (< 60)
  - **Yellow**: Warning levels (60-64)
  - **Red**: Alert levels (≥ 65)
- Configurable number of readings (up to 50)
- Statistical analysis display
- GTK3 graphical user interface

**Main Functions**:
- `main()`: Application entry point and GTK initialization
- `build_gui()`: Constructs the complete user interface

**Compilation Command (use MSYS2 MinGW Terminal)**:
```bash
gcc main.c simulator.c stats.c -o app $(pkg-config --cflags --libs gtk+-3.0)
```

### `test.c`
**Purpose**: Manual testing and CSV generation utility

**How it Works**:

1. **Manual Data Entry Mode**:
   - Prompts user to enter 5 temperature and humidity readings
   - Generates dummy timestamps for each reading

2. **Data Processing**:
   - Applies the same threshold logic as main application:
     - Alert: > 65°C or > 65%
     - Warning: 60-65°C or 60-65%
     - Safe: < 60°C or < 60%

3. **Statistics Calculation**:
   - Calculates averages for temperature and humidity
   - Finds maximum values with corresponding timestamps
   - Uses simple loop-based algorithms

4. **CSV File Generation**:
   - Creates `test_readings.csv` with entered data
   - Format: `seq_no,timestamp,temperature,humidity`

5. **Output Display**:
   - Formatted table showing all readings with expected status and statistical summary
   - Waits for user input to terminate (useful when running test.exe standalone)

**Usage**
```bash
gcc test.c -o test.exe
./test.exe
```

**Testing Workflow**:
1. Run `test.c` to create test data
2. Enter sample temperature/humidity values
3. Generated `test_readings.csv` can be used in main GUI application
4. Select "Test CSV Input" mode in GUI to verify operation

## Compilation Instructions

### Main Application (use MSYS2 MinGW Terminal)
```bash
# Using GCC with pkg-config for GTK3
gcc main.c simulator.c stats.c -o app $(pkg-config --cflags --libs gtk+-3.0)

# Alternative for cross-compilation
x86_64-w64-mingw32-gcc main.c simulator.c stats.c -o app $(pkg-config --cflags --libs gtk+-3.0)
```

### Test Program (use PowerShell)
```bash
gcc test.c -o test.exe
```

## Dependencies

- **GTK3 library**: For GUI development
  Install command: 
  ```bash 
  pacman -S mingw-w64-x86_64-gtk3
  ```
- **MSYS2 Mingw64 terminal in VS Code**: Use the Mingw64 environment
- **Path configurations**:
  ```bash
  C:\msys64\mingw64\bin
  C:\msys64\mingw64\include
  C:\msys64\mingw64\lib
  ```
- **pkg-config**: For library configuration
  Install command: 
  ```bash 
  pacman -S mingw-w64-x86_64-pkg-config
  ```
- **Standard C Libraries**: stdio.h, stdlib.h, string.h, time.h, stdbool.h



## Data Format in CSV Files

CSV files should follow this format if manually created:
```
1,2025-01-01 00:00,25.5,60.2
2,2025-01-01 00:29,26.1,59.8
3,2025-01-01 00:58,24.9,61.5
```

## Thresholds

- **Alert Threshold**: 65.0 (Temperature °C / Humidity %)
- **Warning Threshold**: 60.0 (Temperature °C / Humidity %)
- **Safe Range**: Below 60.0


## Project Structure
```
gtk-only-CO253_Project_Temperature-Humidity-Logger/
├── main.c                    # Main GTK3 application with GUI
├── simulator.c               # Data simulation and CSV handling implementation
├── simulator.h               # Simulator function declarations and Reading structure
├── stats.c                   # Statistical calculations implementation
├── stats.h                   # Statistics function declarations and Statistics structure
├── test.c                    # Manual testing utility and CSV generation
├── readings.csv              # Default simulation data file
├── test_readings.csv         # Generated test data from test.c
├── README.md                 # Project documentation
├── TEST_PLAN.md              # Comprehensive testing documentation
├── libgtk-3-0.dll           
├── libgdk-3-0.dll           
├── libcairo-2.dll           
├── libglib-2.0-0.dll        
└── .vscode/                  # VS Code configuration folder
    ├── c_cpp_properties.json # C/C++ IntelliSense configuration
    ├── tasks.json            # Build tasks configuration
    └── settings.json         # Workspace settings
```

## License

Educational project - Group 08, by E/21/132 , E/21/139
