#ifndef COMMON_UTILITY_FILE_H
#define COMMON_UTILITY_FILE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <ctime>

namespace common_utility {

class FileDate {
public:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;

    FileDate(void) : year(0), month(0), day(0), hour(0), minute(0), second(0) {}

    void print(FILE* a_file) const {
        fprintf(a_file, "%d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
    }
};

class File {
public:
    const std::string path;

    File(const std::string& a_path) : path(a_path) {}

    long get_size(void) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Could not open the file!" << std::endl;
            return -1;
        }
        return file.tellg();
    }

    FileDate get_file_create_time(void) {
        struct stat file_info;
        FileDate file_date;

        if (stat(path.c_str(), &file_info) != 0) {
            std::cerr << "Error: Unable to access file " << path << std::endl;
            return file_date;
        }

        std::tm* create_time = std::localtime(&file_info.st_ctime);
        file_date.year   = create_time->tm_year + 1900;
        file_date.month  = create_time->tm_mon + 1;
        file_date.day    = create_time->tm_mday;
        file_date.hour   = create_time->tm_hour;
        file_date.minute = create_time->tm_min;
        file_date.second = create_time->tm_sec;

        return file_date;
    }

    bool copy(const std::string& a_destination_path) {
        std::ifstream source_file(path, std::ios::binary);
        std::ofstream destination_file(a_destination_path, std::ios::binary);
        if (!source_file.is_open() || !destination_file.is_open()) {
            std::cerr << "Error opening files!" << std::endl;
            return false;
        }
        destination_file << source_file.rdbuf();
        source_file.close();
        destination_file.close();
        return true;
    }
};

}   // namespace common_utility

#endif // COMMON_UTILITY_FILE_H