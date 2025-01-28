#ifndef COMMON_UTILITY_DIRECTORY_H
#define COMMON_UTILITY_DIRECTORY_H

#include <iostream>
#include <string>
#include <list>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <direct.h>
#include <errno.h>
#include "File.hpp"

namespace common_utility {

typedef std::list<File> FileList;

class Directory {
public:
    const std::string path;

    FileList& get_file_list(const std::list<std::string>& a_extensions = m_empty_extensions) {
        m_file_list.clear();
        get_file_list(m_file_list, path, a_extensions);
        return m_file_list;
    }

    bool is_directory(void) {
        struct stat info;
        if (stat(path.c_str(), &info) != 0) {
            return false; // Directory does not exist
        } else if (info.st_mode & S_IFDIR) {
            return true; // Directory exists
        } else {
            return false; // Not a directory
        }
    }

    bool make_directory(void) {
        if (0 == mkdir(path.c_str())) {
            return true;
        }
        std::string error_message = "Fail: mkdir(" + path + ")";
        perror(error_message.c_str());
		fprintf(stderr, "errorno : %d", errno );    
        return false;
    }

private:
    void get_file_list(FileList& a_file_list,
                       const std::string& a_path,
                       const std::list<std::string>& extensions) {
        DIR* dir = opendir(a_path.c_str());
        if (dir == nullptr) {
            std::cerr << "Error opening directory: " << a_path << std::endl;
            return;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) { // If the entry is a regular file
                std::string file_name = entry->d_name;
                if(0 == extensions.size()) {
                    a_file_list.push_back(a_path + "/" + file_name);
                }
                else {
                    for (const auto& extension : extensions) {
                        if (file_name.size() >= extension.size() && 
                            file_name.compare(file_name.size() - extension.size(), extension.size(), extension) == 0) {
                            a_file_list.push_back(a_path + "/" + file_name);
                            break; // No need to check other extensions if one matches
                        }
                    }
                }
            } else if (entry->d_type == DT_DIR) { // If the entry is a directory
                std::string dirName = entry->d_name;
                if (dirName != "." && dirName != "..") {
                    get_file_list(a_file_list, a_path + "/" + dirName, extensions); // Recursively call for subdirectories
                }
            }
        }
        closedir(dir);
    }

    static const std::list<std::string> m_empty_extensions;
    FileList m_file_list;
};

} // namespace common_utility

#endif // COMMON_UTILITY_DIRECTORY_H
