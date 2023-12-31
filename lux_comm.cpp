#include <iostream>
#include <fstream> //Read and write data from a file
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <ctime>
#include <time.h>
#include <bitset>
#include <iomanip>
#include <cstring>//Copying, comparing, and searching strings
#include <cstdint>

using namespace std;
void writeErrortexttolog(const std::string& errorText) {
    std::ofstream journal("task3.log", std::ios::app); // Open log file in append mode
    if (journal) {
        journal << errorText << std::endl;
        journal.close();
    }
}
bool verifyCSVStructure(const std::string& file_name) {
    //// Open the CSV file for reading
    std::ifstream file(file_name);
    // Check if the file was opened successfully
    if (!file) {
        writeErrortexttolog("Error 01: Input file not found or not accessible");
        return false;// Indicate failure
    }
     // Read the header row (first row)
    std::string row;
    if (!std::getline(file, row)) {
        writeErrortexttolog("Error 02: Input file does not have a header");
        return false;
    }
    // Extract header fields (domains) using a stringstream
    std::istringstream ist(row);
    std::string domain ;
    std::vector<std::string> slogan;  // Stores header fields
    while (std::getline(ist, domain, ',')) {
        slogan.push_back(domain); // Add each field to the vector
    }
    // Check if any header fields were found
    if (slogan.empty()) {
        writeErrortexttolog("Error 02: Invalid CSV file format");
        return false;   
    }
    // Initialize variables for checking subsequent rows
    std::string priorrow = row; // Store the previous row
    int headerdomain = slogan.size();
    int countline = 1;
    while (std::getline(file, row)) {
        countline++;
    // Iterate through the remaining rows
        std::istringstream ist(row);
        int Count = 0;
        while (std::getline(ist, domain, ',')) {
            Count++;
        }
         // Check if the number of fields in the current row matches the header
        if (Count != headerdomain) {
            writeErrortexttolog("Error 02: Invalid CSV file format");
            return false;
        }
         // Store the current row for comparison with the next row
        priorrow = row;
    }
    // If all checks pass, the CSV structure is valid
    return true;
}
//verify the existence and accessibility of an output file
bool check_output(const std::string& output) {
    std::ifstream ResultStream(output);
    return ResultStream.good();
}

void Check_File(const std::string& output) {
     // Name of the error log file
    std::string Name_file = "task3.log";
    // Check if the output file already exists
    if (check_output(output)) {
        // Output file exists, write an error message to the error log file
        std::ofstream test_error(Name_file, std::ios::app);
        if (test_error) {
            // Write the error message to the error log
            test_error << "Error 05: cannot override " << output << std::endl;
            test_error.close();
        } else {
            //// Error opening the error log file
            std::cerr << "Failed to open error log file: " << Name_file << std::endl;
        }

        return;
    }
    // Output file does not exist, create a new file
    std::ofstream output_test(output, std::ios::binary);
    if (!output_test) {
       // Error opening the output file
        std::cerr << "Failed to open output file: " << output << std::endl;
        return;
    }
   
    output_test.close();
}
//check string contains only whitespace characters
bool sspace(const std::string& str) {
    return str.find_first_not_of(' ') == std::string::npos;
}
//check format time
bool Value_time(const std::string& time) {
    return (time.length() == 19 &&
            time[4] == ':' && time[7] == ':' && time[10] == ' ' &&
            time[13] == ':' && time[16] == ':');
}

void check_input(const std::string& filename) {
    std::ifstream ssinput(filename);
    std::string row;
    int countline = 0;
    bool find = false;

    // Create error log file
    std::ofstream error_file("task3.log");

    // Process the lines
    while (std::getline(ssinput, row)) {
        countline++;

        std::istringstream ist(row);
        std::string id, time, location,value , condition;

        // Read the fields from the line
        std::getline(ist, id, ',');
        std::getline(ist, time, ',');
        std::getline(ist, location, ',');
        std::getline(ist, value, ',');
        std::getline(ist, condition, ',');

        // Check for errors
        if (countline > 1){
        if (sspace(id) || sspace(time) || sspace(location) || sspace(value) || sspace(condition) ||
            !Value_time(time)) {
            writeErrortexttolog("Error 04: Invalid data format at line " + std::to_string(countline));
            find = true;
        }
        }
    }

    ssinput.close();
}

bool Check_File2(const std::string& ssinput, const std::string& ResultStream) {
    if (ssinput.empty() || ResultStream.empty()) {
        writeErrortexttolog("Error 03: invalid command");
        return false;
    }

    if (!verifyCSVStructure(ssinput)) {
        return false;
    }

    // Rest of the processing logic for files...

    return true;
}
string Start_byte(const string& input) {
    return "A0";
}
string Stop_byte(const string& input) {
    return "A9";
}
string structure_location(uint16_t location) {
    stringstream ss;
    ss << setfill('0')<< location;
    return ss.str();
}


string convert_float(float value) {
    stringstream ss;
    ss << uppercase << hex << setfill('0') << setw(8);

    // Convert float to an integer representation
    uint32_t int_value;
    memcpy(&int_value, &value, sizeof(float));

    // Output the integer value as hexadecimal
    ss << int_value;


    return ss.str();
}

struct Datainput {
    int id;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int location;
    double value;
};

vector<Datainput> Datatake(const string& filename) {
    ifstream file(filename);
    string row;
    vector<Datainput> roll;

    // Skip the header line
    getline(file, row);

    while (getline(file, row)) {
        istringstream ist(row);
        string idString, datetime, locationString,valueString, condition;

        getline(ist, idString, ',');
        getline(ist, datetime, ',');
        getline(ist, locationString, ',');
        getline(ist, valueString, ',');
        getline(ist, condition, ',');

        Datainput data;
        data.id = stoi(idString);
        sscanf(datetime.c_str(), "%4d:%2d:%2d %2d:%2d:%2d",
               &data.year, &data.month, &data.day,
               &data.hour, &data.minute, &data.second);
        data.value = stod(valueString);
        data.location = stoi(locationString);

        roll.push_back(data);
    }

    file.close();

    return roll;
}

string convert(int decimal) {
    string hexadecimal;
    const int x = 16;

    while (decimal > 0) {
        int res = decimal % x;

        if (res < 10)
            hexadecimal += static_cast<char>(res + '0');
        else
            hexadecimal += static_cast<char>(res - 10 + 'A');

        decimal /= x;
    }

    reverse(hexadecimal.begin(), hexadecimal.end());
    return hexadecimal;
}

int  Lux_time(int year, int month, int day, int hour, int min, int sec) {
    struct tm date;
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = day;
    date.tm_hour = hour;
    date.tm_min = min;
    date.tm_sec = sec;
    time_t timemark = mktime(&date);

    return static_cast<int>(timemark);
}
string calculate_checksum(const string& input) {
    unsigned int sum = 0;

    // Iterate through each character pair in the string
    for (size_t i = 0; i < input.length(); i += 2) {
        // Extract the two characters representing a hexadecimal value
        string hexa_value= input.substr(i, 2);

        // Convert the hexadecimal value to an integer
        unsigned int value;
        stringstream ss;
        ss << uppercase << hex << hexa_value;
        ss >> value;

        // Add the value to the sum
        sum += value;
    }

    // Calculate the least significant byte of the sum
    unsigned char least_sb = static_cast<unsigned char>(sum & 0xFF);

    // Calculate the two's complement of the least significant byte
    unsigned char checksum = static_cast<unsigned char>((~least_sb + 1) & 0xFF);

    // Convert the checksum to a string in hexadecimal format
    stringstream result;
    result << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(checksum);
    return result.str();
}

string insertSpace(const string& input) {
    string result;
    int count = 0;
    
    for (char c : input) {
        result += c;
        count++;
        
        if (count % 2 == 0) {
            result += ' ';
        }
    }
    
    return result;
}
string insertzero(const string& input) {
    if (input.length() == 1) {
        return "0" + input;
    }
    
    return input;
}
string calculatePacket_Length(const string& packet) {
    // Return "0E" regardless of the input
    return "0E";
}

int main(int argc, char *argv[]) {
    string filename = argv[1];
    string inputFilename = argv[2];
    check_input(filename);
    Check_File(inputFilename);
    verifyCSVStructure(filename);
    ofstream fileout;
    fileout.open(inputFilename,ios_base::out);
    vector<Datainput> roll = Datatake(filename);
    for (const auto& data : roll) {
        int unixTimestamp = Lux_time(data.year, data.month, data.day, data.hour, data.minute, data.second); 
        string tmp = "0E" + (insertzero(convert(data.id)))+(convert(unixTimestamp))+(convert_float(data.value))+ (structure_location(data.location));
        string tmm = insertSpace(Stop_byte((calculate_checksum(tmp)))) + insertSpace(Start_byte(insertzero(convert(data.id))))+ (insertzero(convert(data.id)))+(convert(unixTimestamp))+(convert_float(data.value))+ (structure_location(data.location));
        fileout << insertSpace(Start_byte(insertzero(convert(data.id))));
        fileout << insertSpace(calculatePacket_Length(tmm));
        fileout << insertSpace(insertzero(convert(data.id)));
        fileout << insertSpace(convert(unixTimestamp));
        if (data.location == 0) {
            fileout << "00 ";
        }else {
            fileout << insertSpace(insertzero(convert(data.location))); 
        }
        fileout << insertSpace(convert_float(data.value));
        fileout << insertSpace(calculate_checksum(tmp));
        fileout << insertSpace(Stop_byte((calculate_checksum(tmp))));
        fileout << endl;
        

    }

   
    fileout.close();
    return 0;
}