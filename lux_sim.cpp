#include <iostream>
#include <fstream>  // For ofstream
#include <string>
#include <ctime>    // For time()
#include <iomanip>  // For setprecision()
#include <vector>
#include <sstream>  // For stringstream
#include <cstdlib>  // For rand()
using namespace std;


// Function to generate a random value within a given range
double generateRandomValue(double min, double max) {
	//rand() function to get a value in range min, max
    double random = static_cast<double>(rand()) / RAND_MAX; //casting it to double and dividing it 
	//by RAND_MAX, you convert the integer 
    //random value to a floating-point value between 0 and 1.
    return min + random * (max - min);
}


// Function to write log messages to a file
void writelog(const string& message) {
    ofstream logfile("task1.log", ios::app);
    if (logfile.is_open()) {
        logfile << message << endl;
    } else {
        cerr << "Error: Unable to open log file 'task1.log'" << endl;
    }
}


//Read how to pass parameter in this link
//https://stackoverflow.com/questions/50021775/command-line-arguments-to-execute-a-c-program
int main(int argc, char* argv[]) {
    // Default values for command-line arguments
    int num_sensors = 1;
    int sampling_time = 60;
    int measurement_duration = 24;

    // Check if the correct number of command-line arguments is provided
    if (argc != 7) {
        writelog("Error 01: Invalid command");
        return 1;
    }

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        // Check for invalid argument values
        if (num_sensors < 1 || sampling_time < 1 || measurement_duration < 1) {
            writelog("Error 02: Invalid argument");
            return 1;
        }

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


    // Open the output file for writing
    ofstream datafile("lux_sensor.csv");
    if (!datafile.is_open()) {
        writelog("Error 03: File access denied");
        return 1;
    }

    // Write header to the output file
    datafile << "id,time,value\n";

    // Generate simulated data
    time_t current_time = time(nullptr);
    time_t start_time = current_time - measurement_duration * 3600;
    srand(static_cast<unsigned int>(time(nullptr)));

    time_t timestamp = start_time;

    // Generate data for each timestamp
    while (timestamp <= current_time) {
        // Get local time information
        struct tm* tm_info = localtime(&timestamp);

        // Format timestamp using stringstream
        stringstream timestamp_ss;
        timestamp_ss << put_time(tm_info, "%Y:%m:%d %H:%M:%S");
        string timestamp_str = timestamp_ss.str();

        // Generate data for each sensor
        for (int i = 1; i <= num_sensors; i++) {
            double measurement = generateRandomValue(0.1, 40000.0);

            // Write data to the output file
            datafile << i << "," << timestamp_str << "," << fixed << setprecision(2) << measurement << "\n";
        }

        // Move to the next timestamp
        timestamp += sampling_time;
    }

    // Close the output file
    datafile.close();

    cout << "Data generated and stored successfully!" << endl;
    return 0;
}
