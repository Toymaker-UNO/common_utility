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
    time_t time;

    FileDate(void) : year(0), month(0), day(0), hour(0), minute(0), second(0), time(0) {}

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

    FileDate get_create_file_date(void) {
        struct stat file_info;
        if (stat(path.c_str(), &file_info) != 0) {
            std::cerr << "Error: Unable to access file " << path << std::endl;
            return get_file_date();
        }
        return get_file_date(file_info.st_ctime);
    }

    FileDate get_modify_file_date(void) {
        struct stat file_info;
        if (stat(path.c_str(), &file_info) != 0) {
            std::cerr << "Error: Unable to access file " << path << std::endl;
            return get_file_date();
        }
        return get_file_date(file_info.st_mtime);
    }

    FileDate get_access_file_date(void) {
        struct stat file_info;
        if (stat(path.c_str(), &file_info) != 0) {
            std::cerr << "Error: Unable to access file " << path << std::endl;
            return get_file_date();
        }
        return get_file_date(file_info.st_atime);
    }

    FileDate get_oldest_file_date(void) {
        FileDate create_date = get_create_file_date();
        FileDate modify_date = get_modify_file_date();
        FileDate access_date = get_access_file_date();
        if (create_date.time <= modify_date.time && create_date.time <= access_date.time) {
            return create_date;
        } else if (modify_date.time <= create_date.time && modify_date.time <= access_date.time) {
            return modify_date;
        }
        return access_date;
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

    bool move(const std::string& a_destination_path) {
        if (rename(path.c_str(), a_destination_path.c_str()) != 0) {
            std::cerr << "Error moving file!" << std::endl;
            return false;
        }
        return true;
    }

    bool remove(void) {
        if (std::remove(path.c_str()) != 0) {
            std::cerr << "Error removing file!" << std::endl;
            return false;
        }
        return true;
    }

    bool exists(void) {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }

    bool compare(const std::string& a_other_path) {
        std::ifstream file1(path, std::ios::binary);
        std::ifstream file2(a_other_path, std::ios::binary);
        if (!file1.is_open() || !file2.is_open()) {
            std::cerr << "Error opening files for comparison!" << std::endl;
            return false;
        }
        std::istreambuf_iterator<char> begin1(file1), end1;
        std::istreambuf_iterator<char> begin2(file2), end2;
        return std::equal(begin1, end1, begin2);
    }

private:
    FileDate get_file_date(const time_t& a_time = 0) {
        FileDate file_date;
        if(0 == a_time) {
            return file_date;
        }

        std::tm* time = std::localtime(&a_time);
        file_date.year   = time->tm_year + 1900;
        file_date.month  = time->tm_mon + 1;
        file_date.day    = time->tm_mday;
        file_date.hour   = time->tm_hour;
        file_date.minute = time->tm_min;
        file_date.second = time->tm_sec;
        file_date.time   = a_time;
        return file_date;
    }
};

}   // namespace common_utility

#endif // COMMON_UTILITY_FILE_H