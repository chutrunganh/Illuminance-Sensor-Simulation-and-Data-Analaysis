#include <iostream>
#include <string>
#include <fstream> //Use with file
#include <sstream> //Use with stringstream
#include <vector>
#include <map>
#include <iomanip> //Use with setprecision
using namespace std;
#define MAX_LINE_LENGTH 30
#define MAX_SENSORS_NUMBER 1000

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

//global variable
//variable to track line index in data file
int lineNumber; //lineNumber start from 0 in log file


//Task 2.1
void filterOutlier(int argc, char* argv[]) {
    lineNumber = 0; //lineNumber start from 0 in log file
    if (argc != 3) {
        writelog("Error 01: Invalid command");
        return;
    }
    string data_File_name = argv[1];

    //read from data file
    ifstream datafile;
    datafile.open(data_File_name);
    if (!datafile){
        writelog("Error 01: input file not found or not accessible");

    }
    if(data_File_name.empty()){
        writelog("Error 03: invalid command");      
        return;
    }
    string line;
    getline(datafile, line); //skip the first/header line
    //check if the header line has correct format
    if(line != "id,time,value"){
        writelog("Error 02: invalid input file format");
        return;
    }

    vector<string> line_in_outlier;
    // we must use line_in-outlines array to store all line that not in range 1 - 30000, since we must print out the
    //numOutliers first, before print out these lines
    int numOutliers = 0;
    while (getline(datafile, line)) {
        stringstream ss(line);  //treat line as a stream character
        string id_str, timestamp, value_str;

        //Read from ss and store to id_str, timestamp, value_str
        getline(ss, id_str, ',');
        getline(ss, timestamp, ',');
        getline(ss, value_str, ',');

        lineNumber++;
        int checkErrors = identifyErrors(id_str, timestamp, value_str);
        if(checkErrors == 0) {//if there is any error, skip this line
            writelog("Error 04 : invalid data at line " + to_string(lineNumber));
            continue;
        }

        double value = stod(value_str);  //only need to comapre sensor value so only convert value_str to double
        if (value < 1 || value > 30000) {
            line_in_outlier.push_back(id_str + ',' + timestamp + ','+ value_str);
            numOutliers++;
        }       
    }
    datafile.close();
    cout <<"Write to outlier file successful." << endl; // For debugging
    
    //write to outliers file
    ofstream outliersFile;
    outliersFile.open("lux_outlier.csv",ios::trunc);   //ios::trunc: delete the content of the file if it exists
    outliersFile << "number of outliers: " << numOutliers << endl; 
    outliersFile << "id,time,value"<< endl;
    for (int i = 0; i < numOutliers; i++) {
        outliersFile << line_in_outlier[i] << endl;
    }

    outliersFile.close();
}


//Task 2.2
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
        case 4:
            out_put = condition(100,150,value);
            break;
        case 5:
            out_put = condition(150,250,value);
            break;
        case 6:
            out_put = condition(200,400,value);
            break;
        case 7:
            out_put = condition(250,350,value);
            break;
        case 8:
            out_put = condition(300,500,value);
            break;
        case 9:
            out_put = condition(500,700,value);
            break;
        case 10:
            out_put = condition(750,850,value);
            break;
        case 11:
            out_put = condition(1500,2000,value);
            break;
        case 12:
            out_put = condition(2000,5000,value);
            break;
        case 13:
            out_put = condition(5000,10000,value);
            break;
        case 14:
            out_put = condition(10000,20000,value);
            break;    

    }

    return out_put;
}

//struct to store each line in condition.csv
struct line_in_condition_struct {
    int id;
    int hour; 
    string timpestamp;
    double value;
    string condition;
};


void min_max_adverage( int argc, char* argv[]) {
    lineNumber = 0; //lineNumber start from 0 in log file
    string location_File_name = argv[2];

    // READ FROM LOCATION FILE, STORE TO locationMap
    vector<int> locationMap(1000,0); //locationMap[id] = location, default all value = 0
    ifstream locationFile;
    locationFile.open(location_File_name);
    
    //check if location file is valid
    if (!locationFile){
        writelog("Error 01: input file not found or not accessible");
    }
    if(location_File_name.empty()){
        writelog("Error 03: invalid command");
        return;
    }  


    string line;
    getline(locationFile, line); //skip the first line
    if(line != "id,location"){
        writelog("Error 02: invalid input file format");
    }

    while (getline(locationFile, line)) {
        int id, location;
        stringstream ss(line);
        //read the id
        getline(ss, line, ',');
        id = stoi(line);
        //read the location
        getline(ss, line, ',');
        if(line.empty()){
            writelog("Error 05 : unknown location of sensor " + to_string(id)); //if location is empty, write to log file
            continue;
        }
        location = stoi(line);

        locationMap[id] = location;
    }
    locationFile.close();

    //print out data from location.csv  // For debugging
    for ( int i = 0; i < sizeof(locationMap); i++) {
        if (locationMap[i] != 0) {
            //cout << i << " " << locationMap[i] << endl;
        }
    }

    //two matrix, the id_per_hour_values matrix store all value of each sensor per hour
    //the id_per_hour_count matrix store the number of value of each sensor per hour appear in data file
    vector< vector<double> > id_per_hour_values(MAX_SENSORS_NUMBER, vector<double>(25, 0)); //maximun 10 id, 24 hours but index from 0 to n-1, hour start from 1 so must use 25
    vector< vector<int> > id_per_hour_count(MAX_SENSORS_NUMBER, vector<int>(25, 0)); //maximun 10 id, 24 hours


    string data_File_name = argv[1];
    //read from data file, filter data by value in range 1 - 30000 and  calculate average 
    //value of each sensors per hour
    ifstream datafile;
    datafile.open(data_File_name);
    if (!datafile){
        writelog("Error 01: input file not found or not accessible");

    }
    vector<int> listOfValidSensors(MAX_SENSORS_NUMBER,0); //store all valid sensors
    int numberOfConditionLines = 0;
    vector<line_in_condition_struct> line_components;
    //array with each element is a line in condition.csv

    string line2;  //indentify with variable "line" declared in line 217
    getline(datafile, line2); //skip the first line
    if(line2 != "id,time,value"){
        writelog("Error 02: invalid input file format");
    }

    while (getline(datafile, line2)) {
        lineNumber++;
        stringstream ss(line2);  //treat line as a stream character
        if(line2.empty()){
            continue;
        }
        string id_str, timestamp, value_str;
        //Read from ss and store to id_str, timestamp, value_str
        getline(ss, id_str, ',');
        getline(ss, timestamp, ',');
        // Extract the hour from the time
        int hour = stoi(timestamp.substr(11, 2));  //take 2 characters from index 11
        getline(ss, value_str, ',');

        int checkErrors = identifyErrors(id_str, timestamp, value_str);
        if(checkErrors == 0) {//if there is any error, skip this line
            continue;
        }

        double value = stod(value_str);  
        if (value >= 1 && value <= 30000) {
            id_per_hour_values[stoi(id_str)][hour] += value;
            id_per_hour_count[stoi(id_str)][hour] ++;
            listOfValidSensors[stoi(id_str)] = 1;
            
            line_in_condition_struct new_line;
            new_line.id = stoi(id_str);
            new_line.hour = hour;
            new_line.timpestamp = timestamp;
            line_components.push_back(new_line);

            //increase count variable
            numberOfConditionLines++;
        }


    }
    datafile.close();


    //print out data from two matrix // For debugging
    // cout << "Matrix one: " << endl;
    // for (int i = 1; i < MAX_SENSORS_NUMBER; i++) {
    //     if (listOfValidSensors[i] == 1) {  //only print out valid sensors
    //         for (int j = 0; j < 24; j++) {
    //             {
    //                 cout << id_per_hour_values[i][j] << " ";
    //             }
    //         }
    //     cout << endl;
    //     }
    // }

    // cout << "Matrix two: " << endl; // For debugging
    // for (int i = 1; i <= MAX_SENSORS_NUMBER; i++) {
    //     if (listOfValidSensors[i] == 1) {  //only print out valid sensors
    //         for (int j = 1; j <= 24; j++) {
    //             {
    //                 cout << id_per_hour_count[i][j] << " ";
    //             }
    //         }
    //     cout << endl;
    //     }
    // }
    //adverage value matrix of each sensor per hour
    //cout << "Adverage value matrix: " << endl;
    vector< vector<double> > id_per_hour_adverage_value(MAX_SENSORS_NUMBER, vector<double>(25, 0));
    for (int i = 1; i <= MAX_SENSORS_NUMBER; i++) {
        if (listOfValidSensors[i] == 1) {
            for (int j = 0; j < 24; j++) {
                if (id_per_hour_count[i][j] != 0) {  //id_per_hour_count[i][j] = 0 means no value of sensor i are measured in hour j
                    id_per_hour_adverage_value[i][j] = id_per_hour_values[i][j] / id_per_hour_count[i][j];
                    //cout << id_per_hour_adverage_value[i][j] << " ";
                }
                else { //do not have any value of sensor i in hour j
                    //cout << "0 ";
                }
            }
        //cout << endl;
        }
    }


    // add condition(print to console)
    // cout<< "Add condition value" <<endl;
    // for (int i = 0; i < numberOfConditionLines; i++) {
    //     //process condition
    //     double adverage_value = id_per_hour_adverage_value[line_components[i].id][line_components[i].hour];
    //     string condition_output = table1(locationMap[line_components[i].id],adverage_value);
    //     cout << line_components[i].id << ',' << line_components[i].timpestamp.substr(0,10) << " " << line_components[i].hour << ":00:00" << ',' 

    //      <<locationMap[line_components[i].id]<< ',' << fixed << setprecision(2) << id_per_hour_adverage_value[line_components[i].id][line_components[i].hour] << ' ' << condition_output << endl;  //upadate this to just show up percision of 2 digits
    // }

    //write to condition file
    ofstream conditionFile;
    conditionFile.open("lux_condition.csv",ios::trunc);   //ios::trunc: delete the content of the file if it exists
    conditionFile << "id,time,location,value,condition"<< endl;
   
    //get the first hour apper in data file, convert to time_t object
    //I read about how to use tm, time_t in c++ in this link: https://copyprogramming.com/howto/c-converting-a-string-to-a-time-t-variable
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
}


//Task 2.3

//struct to store each line in summary.csv
struct line_in_summary_struct {
    int id;
    string max_timpestamp;
    string min_timpestamp;
    string mean_timestamp;
    double max_value;
    double min_value;
    double mean_value;
};


void min_max_mean_allTime(int argc, char* argv[]) {
    lineNumber = 0; //lineNumber start from 0 in log file
    //matrixs to store max, min, mean value of each sensor. matrix[id] = [max,min,mean]
    double id_max_min_mean[MAX_SENSORS_NUMBER][3]; //maximun 1000 id
    for (int i = 0; i < MAX_SENSORS_NUMBER; i++) {
        id_max_min_mean[i][0] = 0; //max value
        id_max_min_mean[i][1] = 999999999999999;   //min value
        id_max_min_mean[i][2] = 0; //mean value
    }

    //count number of value of each sensor
    int id_count[MAX_SENSORS_NUMBER] = {0}; //maximun 1000 id

    string data_File_name = argv[1];
    //read from data file, filter data by value in range 1 - 30000 and  calculate average 
    //value of each sensors per hour
    ifstream datafile;
    datafile.open(data_File_name);
    if (!datafile){
        writelog("Error 01: input file not found or not accessible");
        return;
    }
    vector<int> listOfValidSensors(MAX_SENSORS_NUMBER,0); //store all valid sensors
    int numberOfSummaryLines = 0;
    line_in_summary_struct  line_components[15000]; //each line in summary.csv is a struct 

    string line;  
    getline(datafile, line); //skip the first line
    if(line != "id,time,value"){
        writelog("Error 02: invalid input file format");

    }

    while (getline(datafile, line)) {
        stringstream ss(line);  //treat line as a stream character
        string id_str, timestamp, value_str;
        //Read from ss and store to id_str, timestamp, value_str

        getline(ss, id_str, ',');
        getline(ss, timestamp, ',');
        getline(ss, value_str, ',');

        lineNumber++;
        int checkErrors = identifyErrors(id_str, timestamp, value_str);
        if(checkErrors == 0) {//if there is any error, skip this line
            continue;
        }

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


    //calculate mean value of each sensor
    for (int i = 1; i <= MAX_SENSORS_NUMBER; i++) {
        if (listOfValidSensors[i] == 1) {
            id_max_min_mean[i][2] = id_max_min_mean[i][2] / id_count[i];
            line_components[i].mean_value = id_max_min_mean[i][2];
            // line_components[i].mean_timestamp //process later
        }
    }  

    //find the actual number of sensors
    int numberOfSensors = 0;
    for (int i = 1; i <= MAX_SENSORS_NUMBER; i++) {
        if (listOfValidSensors[i] == 1) {
            numberOfSensors++;
        }
    }


    //find duration
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
    //Fail in case from 23:00:00 to 10:00:00, duration = 11 hours, but in this case, duration = 10 - 23 = -13 hours
    
    //write to summary file
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
}


int main(int argc, char* argv[]) {
    overWriteLogFile(); //overwrite log file every time rerun program
    filterOutlier(argc,argv);
    min_max_adverage(argc,argv);
    min_max_mean_allTime(argc,argv);
    return 0;
 }