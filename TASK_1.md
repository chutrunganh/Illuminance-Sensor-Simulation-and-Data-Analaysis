# Task 1

## Overview

In this section, we introduce the key parameters and the format of the input data required for the task.

### Parameters

- **num_sensor**: This parameter specifies the number of sensors to be created. Sensors will be assigned IDs ranging from 1 to num_sensor<sup>th</sup>.

- **sampling**: This parameter defines the interval, measured in seconds, between each data collection from the sensors.

- **interval**: This parameter sets the total duration of the experiment, measured in hours.

### Data Format

Each line of the input data should be added by the following format: 
```plaintext
id,timestamp,value
```

Where:

- **id**: Represents the unique identifier of the sensor as defined by the `num_sensor` parameter.
  
- **timestamp**: The timestamp is calculated based on the current system time. Initially, the timestamp starts at the current time minus the total experiment duration (`interval`). For subsequent data points, the timestamp is incremented by the value of `sampling` seconds.

```cpp
    time_t current_time = time(nullptr);
    time_t start_time = current_time - measurement_duration * 3600;
    time_t timestamp = start_time;

    // Generate data for each timestamp
    while (timestamp <= current_time) {
        // Get local time information
        struct tm* tm_info = localtime(&timestamp);

        // Format timestamp using stringstream
        stringstream timestamp_ss;
        timestamp_ss << put_time(tm_info, "%Y:%m:%d %H:%M:%S");
        string timestamp_str = timestamp_ss.str();

        for (int i = 1; i <= num_sensors; i++)
        {
            //do something here
        }

        // Move to the next timestamp
        timestamp += sampling_time;
    }
```
- **value**: The value is generated randomly within a given range using the following C++ function:

```cpp
double generateRandomValue(double min, double max) {
	//rand() function to get a value in range min, max
    double random = static_cast<double>(rand()) / RAND_MAX; //casting it to double and dividing it 
	//by RAND_MAX, you convert the integer 
    //random value to a floating-point value between 0 and 1.
    return min + random * (max - min);
}

// Initialize random seed
srand(static_cast<unsigned int>(time(nullptr)));

// Generate simulated measurement value
double measurement = generateRandomValue(0.1, 40000.0); //Range specified in the problem header
```
### Read input from the command line
&emsp;That is a simple explanation of all the requirements for Task 1. Now, let's delve into how to read input from the command line. To execute the simulation with specific parameters, type the following command in the terminal:
```bash
./lux_sim -n 3 -st 60 -si 10
```
Read how to pass parameters into Cpp program in this link: https://stackoverflow.com/questions/50021775/command-line-arguments-to-execute-a-c-program

```cpp
int main(int argc, char* argv[]) {
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        // Parse the value of each command-line argument
        if (arg == "-n" && i + 1 < argc) {
            num_sensors = stoi(argv[i + 1]);
        } 
		else if (arg == "-st" && i + 1 < argc) {
            sampling_time = stoi(argv[i + 1]);
        } 
		else if (arg == "-si" && i + 1 < argc) {
            measurement_duration = stoi(argv[i + 1]);
        }
    }
    //Use for debugging
    // cout << "Number of sensors: " << num_sensors << endl;
    // cout << "Sampling time: " << sampling_time << " seconds" << endl;
    // cout << "Measurement duration: " << measurement_duration << " hours" << endl;
}
```
### Opening and Writing to a File

To write the simulation data to a CSV file, I use the following C++ code snippet:
```cpp
    ofstream datafile("lux_sensor.csv");
    
    datafile << "id,time,value\n";// Write header to the output file

    datafile << "Content to the file" << endln;

    // Close the output file
    datafile.close();
    cout << "Data generated and stored successfully!" << endl;
```
Each line in the output file should be written in this fomat:
```cpp
datafile << i << "," << timestamp_str << "," << fixed << setprecision(2) << measurement << "\n";
```
*Here, we only print the value round up to 2 digits after decimal point, so I use ```fixed << setprecision(2)``` in ```iomanip``` library*

### Complie and Run
&emsp; Now, run the command from the Linux terminal to compile the program:
```bash
gcc lux_sim.cpp
```
Then run it:
```bash
./lux_sim -n 3 -st 60 -si 10
```
&emsp; A message will show up to confirm we have successfully written data to the csv file
```bash
Data generated and store successfully!
```
&emsp;Checking the `lux_sensor.csv` file, it will appear in the same directory as our code and may look something like this:
```csv
id,time,value
1,2023:12:31 05:23:59,895.65
2,2023:12:31 05:23:59,11573.57
3,2023:12:31 05:23:59,35734.77
1,2023:12:31 05:26:59,34121.59
2,2023:12:31 05:26:59,1339.59
3,2023:12:31 05:26:59,32881.15
............
```
### Error handling




