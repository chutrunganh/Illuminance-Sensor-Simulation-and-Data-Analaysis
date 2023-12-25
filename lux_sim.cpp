#include <iostream>
#include <string>
#include <vector>
#include <fstream>  //Use with file
#include <time.h>   //Use with time
using namespace std;

//print out the arguments from the command line
// void printInputfromCommandLine(int argc, char* argv[]) {
//     cout << "argc: " << argc << endl;
//     for (int i = 0; i < argc; i++) {
//         cout << "argv[" << i << "]: " << argv[i]  << endl;
//     }
// }


//generate random value for sensor
double generateRandomValue(double min, double max)
{
    //rand() function to get a value in range min, max
    double random = (double)rand() / RAND_MAX; //casting it to double and dividing it 
    //by RAND_MAX, you convert the integer 
    //random value to a floating-point value between 0 and 1.
    return min + random * (max - min);
}
//Read how to pass parameter in this link
//https://stackoverflow.com/questions/50021775/command-line-arguments-to-execute-a-c-program
int main(int argc, char* argv[]) {

    //Default value
    int num_sensors = 1;
    int sampling_time = 60;
    int measurement_duration = 24;
    //printInputfromCommandLine(argc, argv);
    
    for (int i = 1; i < argc; i++) { //skip the first argument
        
        string arg = argv[i];

        //check for -n
        if (arg == "-n") {
            if (i + 1 < argc) {
                num_sensors = stoi(argv[i+1]);
                if (num_sensors < 1) {
                    cerr << "Error: Number of sensors must be positive." << endl;
                    return 1;
                }
            } else {
                cerr << "Error: Missing value for -n argument." << endl;
                return 1;
            }
        } 
        //check for -st
        else if (arg == "-st") {
            if (i + 1 < argc) {
                sampling_time = stoi(argv[i+1]);
                if (sampling_time < 1) {
                    cerr << "Error: Sampling time must be positive." << endl;
                    return 1;
                }
            } else {
                cerr << "Error: Missing value for -st argument." << endl;
                return 1;
            }
        }
        //check for -si
        else if (arg == "-si") {
            if (i + 1 < argc) {
                measurement_duration = stoi(argv[i+1]);
                if (measurement_duration < 1) {
                    cerr << "Error: Measurement time must be positive." << endl;
                    return 1;
                }
            } else {
                cerr << "Error: Missing value for -si argument." << endl;
                return 1;
            }
        }
    }

    //Use for debugging
    // cout << "Number of sensors: " << num_sensors << endl;
    // cout << "Sampling time: " << sampling_time << " seconds" << endl;
    // cout << "Measurement duration: " << measurement_duration << " hours" << endl;

    FILE *file = fopen("lux_sensor.csv", "w");
    if (file == NULL)
    {
        cerr << "Error: Could not open file." << endl;
        return 1;
    }

    fprintf(file, "id,time,value\n"); //write header

    //Generate simulated data
    time_t current_time = time(NULL);  //get current time of system
    time_t start_time = current_time - measurement_duration * 3600; // Convert duration to seconds, get the starting time
    srand((unsigned int)time(NULL)); // Initialize random seed
    //The srand() function takes an unsigned integer seed value. In this case, the time(NULL) function is used to generate 
    //a seed value based on the current time. This helps to create a sequence of random numbers that are 
    //unpredictable and not easily repeatable.
    time_t timestamp = start_time; // sets the timestamp to the start time

    while (timestamp <= current_time)
    {
        // Generate simulated measurement timestamp writes in format: YYYY:MM:DD hh:mm:ss

        // Declare a pointer to a structure representing time (struct tm) and 
        // use the localtime function to convert the timestamp to a structure holding 
        // the corresponding values for the local time (year, month, day, hour, minute, second).
        struct tm *tm_info = localtime(&timestamp);
        char timestamp_str[20];  // Declare an array of characters to store the formatted timestamp string.

        // Use the strftime function to format the timestamp values stored in tm_info 
        // into a string using the specified format "%Y:%m:%d %H:%M:%S". 
        // This format corresponds to YYYY:MM:DD hh:mm:ss.
        strftime(timestamp_str, sizeof(timestamp_str), "%Y:%m:%d %H:%M:%S", tm_info); //converts the timestamp to a formatted string with the format YYYY:MM:DD hh:mm:ss
        
        for (int i = 1; i <= num_sensors; i++)
        {
            // Generate simulated measurement value
            double measurement = generateRandomValue(0.1, 40000.0);  //given in the header of the problem 0.1 : 40000.0 Lux

            // Write data to file
            fprintf(file, "%d,%s,%.2f\n", i, timestamp_str, measurement);
        }
        timestamp += sampling_time;
    }

    fclose(file);
    cout<< "Data generated and store successfully!" << endl;
    return 0;
}
