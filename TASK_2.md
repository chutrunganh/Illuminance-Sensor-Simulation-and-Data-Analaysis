## Task 2.1:

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
    outliersFile << "number of outliers: " << numOutliers+1 << endl;  //+1 because numOutliers start from 0
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
Now, with the average value in hand, we determine the condition of the lux base on that.The process is as follows: from the ID -> we transfer to the corresponding code/location (given in ```location.csv``` file) -> we get the corresponding min and max values associated with that type of lux (given in **Table 1**). We compare the average value we found above with the min and max to determine the condition.






















### Error Handing
Here, I maintain a global varinal ```linemum``` to track the line we are pcroseccing

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
o Allthedatafieldsinonelineareblank,e.g.:“,,”
o Idisblankorinvalid,e.g.:“-1,2023:11:1100:00:00,50.1” o Timeisblankorinvalid,e.g.:“1,2023:11:1100:00:,50.1” o Luxvalueisblank,e.g.:“1,2023:11:1100:00:00,”
The error message must include the line number in the input file in which the error happens, i.e.: “Error 04: invalid data at line X” where X is the line number. The first line in the input file which is the header line is line 0 (zero), e.g.: in data_filename.csv the line “id,time,value” is the header line, the next lines onwards are data line and are numbered from 1 (one).
The program should then ignore the line with wrong data and continue to process next line when perform task 2.
 If a sensor in data file is not included in the location file or the line including the location
information of that sensor in location file has wrong data, e.g. data file has 9 sensors with ids from 1 to 9 but the location file has only 7 sensor ids 1, 2, 5, 6, 7, 8, 9 or location code in location.csv is blank, e.g.: “3,”. The error message can be “Error 05: unknown location of sensor ID” where ID is the id of the sensor which does not presented in location file.
