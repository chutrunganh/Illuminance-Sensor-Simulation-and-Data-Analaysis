## Task 2.1: Filtering Invalid Data

### Overview
The objective of this task is to read data from the `lux_sensor.csv` file, filter out the outliers based on the lux level range of 1-30000, and then store the outliers in a new CSV file named `lux_outlier.csv`.

### Implementation Steps

1. **Reading Data from File**
   
    Read each line from the data file, extract the value, and check if it falls within the range **1 - 30000**. Then store invalid lines in an array ```line_in_outlier```.

    ```cpp
    string data_File_name = argv[1];

    //read from data file
    ifstream datafile;
    datafile.open(data_File_name);
   
    string line;
    getline(datafile, line); //skip the first/header line
    
    vector<string> line_in_outlier;
    int numOutliers = 0;
    // we must use line_in_outlier array to store all line that not in range 1 - 30000, since we must print out the
    //numOutliers first, before print out these lines
    
    while (getline(datafile, line)) {
        stringstream ss(line);  //treat line as a stream character
        string id_str, timestamp, value_str;

        //Read from ss and store to id_str, timestamp, value_str
        getline(ss, id_str, ',');
        getline(ss, timestamp, ',');
        getline(ss, value_str, ',');

        double value = stod(value_str); 
        if (value < 1 || value > 30000) {
            line_in_outlier.push_back(id_str + ',' + timestamp + ','+ value_str);
            numOutliers++;
        }       
    }
    datafile.close();
    cout <<"Write to outlier file successful." << endl; 
    ```
   
2. **Writing to Output File**
   - Write the number of outliers and the outlier data to `lux_outlier.csv`.

    ```cpp
    ofstream outliersFile;
    outliersFile.open("lux_outlier.csv",ios::trunc);   //ios::trunc: delete the content of the file if it exists
    outliersFile << "number of outliers: " << numOutliers << endl;  
    outliersFile << "id,time,value"<< endl;
    for (int i = 0; i < numOutliers; i++) {
        outliersFile << line_in_outlier[i] << endl;
    }

    outliersFile.close();
    ```

### Complie and Run
After implementing the code, complie and execute the program from the terminal using the following command:
```bash
./lux_process lux_sensor.csv location.csv
```

Upon successful execution, a message will show up to confirm
```plaintext
Write to outlier file successful.
```
Also, check the lux_outlier.csv file in the same directory. An example of the output file content is as follows:
```csv
number of outliers: 51
id,time,value
3,2023:12:31 05:23:59,35734.77
1,2023:12:31 05:26:59,34121.59
3,2023:12:31 05:26:59,32881.15
1,2023:12:31 05:29:59,33226.98
```
## Task 2.2: Calculating Average Sensor Values per Hour and Determining Lux Conditions

### Overview
In this task, the objective is to calculate the average value of each sensor for each hour and determine the condition of the lux based on predefined thresholds. The process involves multiple steps, including data reading, processing, and condition determination.

### Implementation

First, start with average value. Given a pice of data in  ```lux_sensor.csv``` as follow:
```csv
1,2023:12:01 13:58:46,17801.36
2,2023:12:01 13:58:46,35625.74
3,2023:12:01 13:58:46,9690.57
1,2023:12:01 13:59:46,6799.24
2,2023:12:01 13:59:46,25424.52
3,2023:12:01 13:59:46,889.80
1,2023:12:01 14:00:46,26577.63
2,2023:12:01 14:00:46,36774.66
3,2023:12:01 14:00:46,34386.15
```
For example, suppose we need to calculate the average value of sensor ID 1 during the 13<sup>th</sup> hour(all lines with hour in range 12:00:01 to 13:00:00). To achieve this, we collect all the values from sensor ID 1 recorded during the 13th hour. Specifically, we consider values like **17801.36** and **6799.24** (excluding **26577.63** since it corresponds to the 14<sup>th</sup> hour).

To facilitate this calculation, I utilize two 2D matrices:

```cpp
vector< vector<double> > id_per_hour_values(MAX_SENSORS_NUMBER, vector<double>(25, 0)); 
vector< vector<int> > id_per_hour_count(MAX_SENSORS_NUMBER, vector<int>(25, 0)); 
```

The ```id_per_hour_values``` matrix accumulates the values for each sensor ID with each hour, while the ```id_per_hour_count``` matrix tracks the frequency of each value.

The value of ```MAX_SENSORS_NUMBER``` is set equal to 1000 by default. Also, you might wonder why the second dimension of the matrices is 25 ?. Since a  day only  has a 24 hours, we need 24 columns. However, since indexing starts at 0 and the first hour is represented by index 1, we require **25** columns in total.

Additionally, I employ a vector: ```vector<int> listOfValidSensors(MAX_SENSORS_NUMBER,0)```, to mark the sensor IDs that actually appear in the data. This ensures that when we display the matrices, only the rows corresponding to valid sensor IDs are printed, avoiding unnecessary output for nonexistent sensors.

To process the data file, I employ a struct to capture relevant details for futher process:

```cpp
//struct to store each line in condition.csv
struct line_in_condition_struct {
    int id;
    int hour; 
    string timpestamp;
    double value;
    string condition;
};

vector<line_in_condition_struct> line_components;
//array with each element is a line in condition.csv
```

Now, we read the data and extract the necessary information (ID, hour from the timestamp, and value), then process this data, assigning it to ```line_components[i]``` and updating two matrices in the corresponding cell for further processing of the average value. Also, remember that only valid values in range 1 - 30000 need to be process.

```cpp
    vector< vector<double> > id_per_hour_values(MAX_SENSORS_NUMBER, vector<double>(25, 0));     
    vector< vector<int> > id_per_hour_count(MAX_SENSORS_NUMBER, vector<int>(25, 0)); 

    string data_File_name = argv[1];
    ifstream datafile;

    vector<int> listOfValidSensors(MAX_SENSORS_NUMBER,0); //store all valid sensors
    int numberOfConditionLines = 0;
    vector<line_in_condition_struct> line_components;
    //array with each element is a line in condition.csv

    string line2;  
    getline(datafile, line2); //skip the first line
    while (getline(datafile, line2)) {
        stringstream ss(line2);  //treat line as a stream character
        string id_str, timestamp, value_str;
        //Read from ss and store to id_str, timestamp, value_str
        getline(ss, id_str, ',');
        getline(ss, timestamp, ',');
        // Extract the hour from the time
        int hour = stoi(timestamp.substr(11, 2));  //take 2 characters from index 11
        
        getline(ss, value_str, ',');
        double value = stod(value_str);
         
        if (value >= 1 && value <= 30000) {
            //update matrix
            id_per_hour_values[stoi(id_str)][hour] += value;
            id_per_hour_count[stoi(id_str)][hour] ++;
            listOfValidSensors[stoi(id_str)] = 1;
            
            //assign
            line_in_condition_struct new_line;
            new_line.id = stoi(id_str);
            new_line.hour = hour;
            new_line.timpestamp = timestamp;
            line_components.push_back(new_line);

            //increase count variable
            numberOfConditionLines++;
        }
    }
```
After filling the two matrices, I will print them out to ensure we are still on the right track. Remember, we use ```listOfValidSensors``` to only print out valid rows.
```cpp
    // For debugging only
    cout << "Matrix one: " << endl;
    for (int i = 1; i < MAX_SENSORS_NUMBER; i++) {
        if (listOfValidSensors[i] == 1) {  //only print out valid sensors
            for (int j = 0; j < 24; j++) {
                {
                    cout << id_per_hour_values[i][j] << " ";
                }
            }
        cout << endl;
        }
    }

    cout << "Matrix two: " << endl; 
    for (int i = 1; i <= MAX_SENSORS_NUMBER; i++) {
        if (listOfValidSensors[i] == 1) {  //only print out valid sensors
            for (int j = 1; j <= 24; j++) {
                {
                    cout << id_per_hour_count[i][j] << " ";
                }
            }
        cout << endl;
        }
    }
```
Example with data file:      
```csv
id,time,value
1,2023:12:01 13:41:46,6138.05
2,2023:12:01 13:41:46,27731.97
3,2023:12:01 13:41:46,37530.45
1,2023:12:01 13:42:46,20043.72

```

```csv
Matrix one: 
0 0 0 0 0 0 0 0 0 0 0 0 193396 704972 595565 604828 517070 753593 723174 669346 573098 575122 468640 0 
0 0 0 0 0 0 0 0 0 0 0 0 196741 691700 467868 708650 767981 688022 723511 808505 662875 662032 462077 0 
0 0 0 0 0 0 0 0 0 0 0 0 212568 650499 650276 655114 644167 524465 732504 755244 756081 788001 512939 0 
Matrix two: 
0 0 0 0 0 0 0 0 0 0 0 0 14 42 44 43 38 43 43 47 36 47 33 0 
0 0 0 0 0 0 0 0 0 0 0 0 12 45 34 48 48 46 46 51 47 40 32 0 
0 0 0 0 0 0 0 0 0 0 0 0 14 44 44 47 43 42 46 46 46 46 32 0 
```
Seems correct. Afterward, we calculate the average value. I use a matrix ```vector< vector<double> > id_per_hour_adverage_value(MAX_SENSORS_NUMBER, vector<double>(25, 0))``` to store the average value by corresponding ID and hour:
```cpp
    cout << "Adverage value matrix: " << endl;
    vector< vector<double> > id_per_hour_adverage_value(MAX_SENSORS_NUMBER, vector<double>(25, 0));
    for (int i = 1; i <= MAX_SENSORS_NUMBER; i++) {
        if (listOfValidSensors[i] == 1) {
            for (int j = 0; j < 24; j++) {
                if (id_per_hour_count[i][j] != 0) {  //id_per_hour_count[i][j] = 0 means no value of sensor i are measured in hour j
                    id_per_hour_adverage_value[i][j] = id_per_hour_values[i][j] / id_per_hour_count[i][j];
                    cout << id_per_hour_adverage_value[i][j] << " ";
                }
                else { //do not have any value of sensor i in hour j
                    cout << "0 ";
                }
            }
        cout << endl;
        }
    }
```
Print out the average matrix:
```csv
Adverage value matrix: 
0 0 0 0 0 0 0 0 0 0 0 0 13814 16785.1 13535.6 14065.8 13607.1 17525.4 16818 14241.4 15919.4 12236.6 14201.2 0 
0 0 0 0 0 0 0 0 0 0 0 0 16395.1 15371.1 13760.8 14763.5 15999.6 14957 15728.5 15853 14103.7 16550.8 14439.9 0 
0 0 0 0 0 0 0 0 0 0 0 0 15183.4 14784.1 14779 13938.6 14980.6 12487.3 15924 16418.3 16436.5 17130.5 16029.4 0 
``` 
Now, with the average value in hand, we determine the condition of the lux base on that. The process is as follows: from the ID -> we transfer to the corresponding code/location (given in ```location.csv``` file) -> we get the corresponding min and max values associated with that type of lux (given in **Table 1**). We compare the average value we found above with the min and max to determine the condition.

** Step by Step **

First, we get the code/location of an ID given in ```location.csv```. For example, the file look as follow:
```csv
id,location
1,3
3,4
4,12
5,8
6,9
7,14
8,10
```
Now, we read the ```location.csv``` file and use a vector ```locationMap``` to store IDs with corresponding locations (```locationMap[id] = location```):

```cpp
    string location_File_name = argv[2];

    // READ FROM LOCATION FILE, STORE TO locationMap
    vector<int> locationMap(1000,0); //locationMap[id] = location, default all value = 0
    ifstream locationFile;
    locationFile.open(location_File_name);
    
    string line;
    getline(locationFile, line); //skip the first line
    while (getline(locationFile, line)) {
        int id, location;
        stringstream ss(line);
        //read the id
        getline(ss, line, ',');
        id = stoi(line);
        //read the location
        getline(ss, line, ',');
        location = stoi(line);

        locationMap[id] = location;
    }
    locationFile.close();
```
Print out the vector to ensure that we are still on the right track
```cpp
   // For debugging only
    for ( int i = 0; i < sizeof(locationMap); i++) {
        if (locationMap[i] != 0) {
            cout << i << " " << locationMap[i] << endl;
        }
    }
```
After obtaining the correct location/code, we map it to the corresponding min and max values given in ```Table 1``` and output the condition of the corresponding location:

```cpp
string condition(int min, int max ,double value) {
    if (value < min) {
        return "dark";
    }
    else if (value > max) {
        return "bright";
    }
    else return "good";
}

string table1(int location, double value) {
    string out_put;
    switch (location) {  //modify these value later. why valid range is 1 -30000 but table only 20 -20000
        case 0:  
            out_put =  "NA";
            break;
        case 1:
            out_put = condition(20,50,value);
            break;
        case 2:
            out_put = condition(50,100,value);
            break;
        case 3:
            out_put = condition(100,200,value);
            break;
        ...........
    }
    return out_put;
}
```

Then, we add the output condition to each corresponding line, completing the structure and preparing to write to ```lux_condition.csv```. Since we need to print the average value of each sensor for each hour, we will start from the ***first hour*** in the data file. We then iterate through all valid sensors, print their average value, and proceed to the next hour. Here is the pseudo-code to illustrate this process:

```cpp
    ofstream conditionFile;
    conditionFile.open("lux_condition.csv",ios::trunc);   
    conditionFile << "id,time,location,value,condition"<< endl;
   
    current_hour = first_hour;
    while (current_hour <= last_hour) {  

        for (int id = 1; id <= MAX_SENSORS_NUMBER; id++) { //interate through valid sensors
            if (listOfValidSensors[id] == 1) {
                if (id_per_hour_count[id][current_hour] != 0) {
                    double average_value = id_per_hour_adverage_value[id][current_hour];
                    string condition_output = table1(locationMap[id], average_value);

                    //write to condition file
                    conditionFile << id << ',' << timestamp << ',' << locationMap[id] << ',' << std::fixed << setprecision(2) << average_value << ',' << condition_output << endl;
                }
            }
        }
        // Increment the hour
        current_hour += 1
    }
    conditionFile.close();
    cout << "Write to condition file successful." << endl; 
```
But the problem here is how to compare two hour (start and last hour). We can not extract the hour from the fisrt and last time stamp, since there my be a case: start time 23:00:00 today to last time 5:00:00 tomorrow, that will make compare by pure hour become useless. Therefore, I use time_t and tm  data structure to cpmare between them.
***I read about how to use tm, time_t in c++ in this link: https://copyprogramming.com/howto/c-converting-a-string-to-a-time-t-variable***
```cpp
   
    //get the first hour apper in data file, convert to time_t object
    string tmp_first_time_stamp = line_components[0].timpestamp;
    tm tm_first = {};

    tm_first.tm_year = std::stoi(tmp_first_time_stamp.substr(0, 4)) - 1900;  // Year since 1900
    tm_first.tm_mon = std::stoi(tmp_first_time_stamp.substr(5, 2)) - 1;      // Month (0-based)
    tm_first.tm_mday = std::stoi(tmp_first_time_stamp.substr(8, 2));        // Day
    tm_first.tm_hour = std::stoi(tmp_first_time_stamp.substr(11, 2));       // Hour
    // why we must subtract 1900: https://stackoverflow.com/questions/55211776/my-question-is-about-using-1900-in-tm-year

    time_t first_time_stamp = mktime(&tm_first);

    
    //get the last hour apper in data file
    string tmp_last_time_stamp = line_components[numberOfConditionLines-1].timpestamp;
    tm tm_last = {};
    tm_last.tm_year = std::stoi(tmp_last_time_stamp.substr(0, 4)) - 1900;  // Year since 1900
    tm_last.tm_mon = std::stoi(tmp_last_time_stamp.substr(5, 2)) - 1;      // Month (0-based)
    tm_last.tm_mday = std::stoi(tmp_last_time_stamp.substr(8, 2));        // Day
    tm_last.tm_hour = std::stoi(tmp_last_time_stamp.substr(11, 2));       // Hour

    time_t last_time_stamp = mktime(&tm_last);

    time_t current_time_stamp = first_time_stamp;
    tm *local_time;

    while (current_time_stamp <= last_time_stamp) {  //tm can not compare with each other, so must convert to time_t
        local_time = localtime(&current_time_stamp);  //convert back to tm object to extract hour
        //Do not assign int hour = local_time->tm_hour; because it  will make 08 hour become 8 hour

        for (int id = 1; id <= MAX_SENSORS_NUMBER; id++) {
            if (listOfValidSensors[id] == 1) {
                if (id_per_hour_count[id][local_time->tm_hour] != 0) {
                    double average_value = id_per_hour_adverage_value[id][local_time->tm_hour];
                    string condition_output = table1(locationMap[id], average_value);

                    //write to condition file
                    conditionFile << id << ',' << local_time->tm_year + 1900 << ":"<< local_time->tm_mon <<":" << local_time->tm_mday  << " " << local_time->tm_hour + 1 << ":00:00"
                                  << ',' << locationMap[id] << ',' << std::fixed << std::setprecision(2) << average_value
                                  << ',' << condition_output << std::endl;
                }
            }
        }

        // Increment the hour
        current_time_stamp += 3600;  // 3600 seconds = 1 hour
    }
    conditionFile.close();
    cout << "Write to condition file successful." << endl; // For debugging 
```
### Complie and Run
Run the program and the ```luc_condition.csv``` should look something like this:
```csv
id,time,location,value,condition
1,2024:0:2 20:00:00,3,12992.49,bright
2,2024:0:2 20:00:00,0,16678.92,NA
3,2024:0:2 20:00:00,7,14678.67,bright
1,2024:0:2 21:00:00,3,16247.99,bright
2,2024:0:2 21:00:00,0,17931.26,NA
3,2024:0:2 21:00:00,7,15804.86,bright
......................
```
## TASK 2.3 Calculating and Summarizing Sensor Data
### Over View
In this task, our objective is to compute and summarize the **maximum**, **minimum**, and **mean values** —associated with each sensor's readings ***over the entire dataset duration***.

For each sensor under consideration, the analysis will be presented in a structured format comprising three distinct lines:

1. **Maximum Value Analysis**: This will include the maximum value detected, along with the timestamp indicating when this maximum value was first observed.

2. **Minimum Value Analysis**: Similarly.

3. **Mean Value Analysis**: This segment will provide insights into the duration of the expermental and also the overall mean value derived from the sensor's readings throughout that period of time.

### Data Structures:
Initialy, I use a two-dimensional matrix, with each row corresponding to a unique sensor and each column representing a specific properties of thar sensor:
- Column 0: Maximum value
- Column 1: Minimum value
- Column 2: Mean value
The matrix is defined with default values as follow::

```cpp
double id_max_min_mean[MAX_SENSORS_NUMBER][3]; 
for (int i = 0; i < MAX_SENSORS_NUMBER; i++) {
    id_max_min_mean[i][0] = 0; // Initialize max value
    id_max_min_mean[i][1] = 999999999999999; // Initialize min value
    id_max_min_mean[i][2] = 0; // Initialize mean value
}
```
Also, an array ```id_count[sensors]``` to count how many value does each sensor has. Through each interation, we accumulate values for each sensor and then combine with this array to obtain the mean value of each sensor

Then, I use a struct to hold the structure of lines in the output file.
```cpp
    struct line_in_summary_struct {
    int id;
    string max_timpestamp;
    string min_timpestamp;
    string mean_timestamp;
    double max_value;
    double min_value;
    double mean_value;
};
line_in_summary_struct  line_components[15000]; //each line in summary.csv is a struct 
```
### Data Processing Steps:
Now we read data file, and for each input line, we store the maximum and minimum values of each sensor, along with the corresponding timestamps, in the respective matrix cells (We only need to store the timestamps for the maximum and minimum values; the duration for the mean value will be calculated later). Then, assign these values to ```line_components[i]```.
```cpp
    //count number of value of each sensor
    int id_count[MAX_SENSORS_NUMBER] = {0}; 

    string data_File_name = argv[1];
    //read from data file, filter data by value in range 1 - 30000
    
    ifstream datafile;
    datafile.open(data_File_name);
    
    vector<int> listOfValidSensors(MAX_SENSORS_NUMBER,0); //store all valid sensors
    int numberOfSummaryLines = 0;
    line_in_summary_struct  line_components[15000]; //each line in summary.csv is a struct 

    string line;  
    getline(datafile, line); //skip the first line

    while (getline(datafile, line)) {
        stringstream ss(line);  //treat line as a stream character
        string id_str, timestamp, value_str;
        //Read from ss and store to id_str, timestamp, value_str

        getline(ss, id_str, ',');
        getline(ss, timestamp, ',');
        getline(ss, value_str, ',');
        int id = stod(id_str);      
        double value = stod(value_str);  

        if (value >= 1 && value <= 30000) {
            //update max, min, mean value of each sensor
            if (value > id_max_min_mean[id][0]) {
                id_max_min_mean[id][0] = value;
                line_components[id].max_value = value;
                line_components[id].max_timpestamp = timestamp;
            }
            if (value < id_max_min_mean[id][1]) {
                id_max_min_mean[id][1] = value;
                line_components[id].min_value = value;
                line_components[id].min_timpestamp = timestamp;
            }
            //update mean value of each sensor
            id_max_min_mean[id][2] += value;
            id_count[id] ++;

            listOfValidSensors[id] = 1;
            //increase count variable
            numberOfSummaryLines++;
        }
    }
    datafile.close();
```
Now, after accumulate values of each sensor all the time, we calculate the mean value, reassign it right in that corresponding cell (```id_max_min_mean[i][2]```):
```cpp
for (int i = 1; i <= MAX_SENSORS_NUMBER; i++) {
        if (listOfValidSensors[i] == 1) {
            id_max_min_mean[i][2] = id_max_min_mean[i][2] / id_count[i];
            line_components[i].mean_value = id_max_min_mean[i][2];
            // line_components[i].mean_timestamp //process later
        }
    }
```
Move on to find the timestamp/ **duration** of mean value, we take the fisrt hour - last hour in the data file:

```cpp
    datafile.open(data_File_name);
    getline(datafile, line); //skip the first line
    //read the second line
    getline(datafile, line);
    stringstream secondLineStream(line);  //treat line as a stream character
    string id_str_temp, timestampFirst_temp, value_str_temp;
    //id_str has been declared above, so use id_str_temp instead
    getline(secondLineStream, id_str_temp, ',');
    getline(secondLineStream, timestampFirst_temp, ',');
    getline(secondLineStream, value_str_temp, ',');
    int hourFirst = stoi(timestampFirst_temp.substr(11, 2)); // Extract hour from first line

    //read the last line
    string lastLine;
    while (getline(datafile, line)) {
        lastLine = line;
    }
    stringstream lastLineStream(lastLine);  //treat line as a stream character
    //string id_str_temp, timestampLast_temp, value_str_temp;
    string timestampLast_temp;
    getline(lastLineStream, id_str_temp, ',');
    getline(lastLineStream, timestampLast_temp, ',');
    getline(lastLineStream, value_str_temp, ',');
    int hourLast = stoi(timestampLast_temp.substr(11, 2)); 

    datafile.close();
    
    int duration = hourLast - hourFirst; //duration in hours 
```
***However, this code will fail in case from 23:00:00 to 10:00:00, duration = 11 hours, but in this case, duration = 10 - 23 = -13 hours (UPDATE this later)***
You can run this piece of code and check. It must be the same as the third parameter from the command line you used in Task 1. After we have all the components we need, let's write to ```lux_summary.csv```:

```cpp
    ofstream outputfile;
    outputfile.open("lux_summary.csv");
    outputfile << "id,parameter,time,value" << endl;
    for (int i = 1; i <= numberOfSensors; i++) {
        //id, max,timestap, value
        //id, min, timestamp, value
        //id, mean, timestamp, value

        //with this, can print out with two digits after decimal point
        outputfile << i << ", max," << line_components[i].max_timpestamp << ','
            << fixed << setprecision(2) << line_components[i].max_value << '\n'
            << i << ", min," << line_components[i].min_timpestamp << ','
            << fixed << setprecision(2) << line_components[i].min_value << '\n'
            << i << ", mean," << duration << ":00:00,"
            << fixed << setprecision(2) << line_components[i].mean_value << endl;
    }
    outputfile.close();
    cout << "Write to summary file successful." << endl; 
```
### Main function
Now we run three parts at once:
```cpp
int main(int argc, char* argv[]) {
    overWriteLogFile(); 
    filterOutlier(argc,argv);
    min_max_adverage(argc,argv);
    min_max_mean_allTime(argc,argv);
    return 0;
 }
```
*use ```overWriteLogFile()``` to overwrite any exist log files every time rerun the program.

### Error Handing
Here, I maintain a global variable ```linemum``` to track the line in data file we are pcroseccing since the error messages require this parameter.

Errors may happen in task 2:
- The input file data_filename.csv or location.csv does not exist or is not allowed to access. The error message can be “Error 01: input file not found or not accessible”
    ```cpp
    string data_File_name = argv[1];
    datafile.open(data_File_name);
    if (!datafile){
        writelog("Error 01: input file not found or not accessible");

    }
    //Simiar to location.csv
    ```

- The input file data_filename.csv or location.csv does not have proper format as required, e.g. it has no header file, wrong number of comma, or even consists of completely different data
format. The error message can be “Error 02: invalid input file format”

    ```cpp
    if(line != "id,time,value"){
        writelog("Error 02: invalid input file format");
        return;
    }
    ```
- The user types the wrong command-line format, e.g.: one or both the two filenames are
missing. “Error 03: invalid command”

    ```cpp
    if(data_File_name.empty()){
        writelog("Error 03: invalid command");      
        return;
    }
    ```
- The input data file contains wrong data:
    - All the data fields in one line are blank,e.g.:“,,”
    - Id is blank or invalid,e.g.:“-1,2023:11:11 00:00:00,50.1” 
    - Time is blank or invalid,e.g.:“1,2023:11:11 00:00:,50.1”  
    - Lux value is blank,e.g.:“1,2023:11:11 00:00:00,”
The error message must include the line number in the input file in which the error happens, i.e.: “Error 04: invalid data at line X” where X is the line number. 
    ```cpp
    void overWriteLogFile(){
        ofstream logfile;
        logfile.open("task2.log", ios::trunc); //ios::trunc: delete the content of the file if it exists
        logfile.close();
    }

    void writelog(const string& message){
        ofstream logfile;
        logfile.open("task2.log", ios::app); //ios::app: append to the end of file
        logfile << message << endl;
        logfile.close();
    }

    bool identifyErrors(string id, string time, string value) {
        int count = 0;

        if (id.empty()){
            ++count;
        }
        else if (stoi(id) < 1){
            ++count;
        }
        if (time.empty() || time.length() != 19){
            ++count;
        }
        if (value.empty()){
            ++count;
        }
        if (count > 0){ //if there is any error, return false
            return false;
        }
        return true;

    }
    ```

- If a sensor in data file is not included in the location file or the line including the location
information of that sensor in location file has wrong data, e.g. data file has 9 sensors with ids from 1 to 9 but the location file has only 7 sensor ids 1, 2, 5, 6, 7, 8, 9 or location code in location.csv is blank, e.g.: “3,”. The error message can be “Error 05: unknown location of sensor ID” where ID is the id of the sensor which does not presented in location file.

    ```cpp
    if(line.empty()){
            writelog("Error 05 : unknown location of sensor " + to_string(id)); //if location is empty, write to log file
            continue;
        }
    ```

***The program should then ignore the line with wrong data and continue to process next line when perform task 2.***
