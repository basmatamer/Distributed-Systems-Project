#include "Configs.h"
#include "globalvar.h"
#include "proxyFunctions.hpp"

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#include <memory>
#include <dirent.h>
#include <cstring>

#include <QFile>
#include <QMessageBox>
#include <QCoreApplication>

#include "ourexception.h"

std::string home_directory;
std::string user_username_string;
bool has_internet;

//std::map<ImageReferenceMap, std::mutex*> locks_on_images;


void attemptToLogout(QWidget * from_window) {
//    QCoreApplication::quit();



    if(has_internet)
    {
        bool did_end_server;
        try {
            did_end_server = proxyFunctions::logout(myClient);
        } catch(const OurException& except) {
            std::cout << "AttemptToLogout: " << except.what() << std::endl;
             QCoreApplication::quit();
        }
        if(did_end_server) {


            // TODO
            // delete all global variables
            delete myClient;
            myClient = NULL;
//            for(std::map<ImageReferenceMap, std::mutex*>::iterator it = locks_on_images.begin(); it != locks_on_images.end(); ++it) {
//                delete it->second;
//            }
//            locks_on_images.clear();
            // return user to main page

            std::cout << "Deleting client" << std::endl;

            std::cout << "Attempting to delete a window" << std::endl;
            from_window->deleteLater();
            login_window->show();

        } else {
            QMessageBox::warning(from_window,"Error", "Unable to safely logout.\nPlease try again later.");
        }
   }
    else
    {
//        for(std::map<ImageReferenceMap, std::mutex*>::iterator it = locks_on_images.begin(); it != locks_on_images.end(); ++it) {
//            delete it->second;
//        }
//        locks_on_images.clear();
        QCoreApplication::quit();
    }
}

void setHomeDirectory() {
    struct passwd *pw = getpwuid(getuid());
    home_directory = pw->pw_dir;
    home_directory += "/";
}

bool checkDirectoryExists(const std::string& path) {
    struct stat statbuf;
    std::cout << "Checking this path: " << path << std::endl;
    if (stat(path.c_str(), &statbuf) != -1) {
       return (S_ISDIR(statbuf.st_mode));
    } else {
        return false;
    }
}
bool checkAppPath() {
    return checkDirectoryExists(app_path);
}
bool checkDefImgPath() {
    std::ifstream test_input;
    std::string path_to_check = app_path + "def.jpg";
    test_input.open(path_to_check);
    if(test_input.fail()) {
        return false;
    } else {
        test_input.close();
        return true;
    }
}

void createFolder(const std::string& path_to_folder_to_create) {
    if (mkdir(path_to_folder_to_create.c_str(), 0777) == -1) {
        std::cerr << "Error :  " << strerror(errno) << std::endl;
    } else {
        std::cout << path_to_folder_to_create << " created" << std::endl;
    }
}

void createUserFolder() {
    createFolder(defpath1);
    createFolder(owned_resources_path);
    createFolder(icons_path);
  //  createFolder(sharedicons_path);
    createFolder(owned_images_path);
    createFolder(shared_resources_path);
    createFolder(shared_images_path);
    createFolder(files_cache_path);

    QFile::copy(QString::fromStdString(app_path+"def.jpg"), QString::fromStdString(owned_images_path+"def.jpg"));
}

std::vector<std::string> GetDirectoryFiles(const std::string& dir) {
    std::vector<std::string> files;
    files.clear();
    std::shared_ptr<DIR> directory_ptr(opendir(dir.c_str()), [](DIR* dir){ dir && closedir(dir); });
    struct dirent *dirent_ptr;
    if (!directory_ptr) {
        std::cout << "Error opening : " << std::strerror(errno) << dir << std::endl;
        return files;
    }

    while ((dirent_ptr = readdir(directory_ptr.get())) != nullptr) {
        std::string new_path = std::string(dirent_ptr->d_name);
        if(new_path.length() > 0) {
            // ignore hidden files
            if(new_path[0] != '.') {
//                std::cout << "A new path is: " << new_path << std::endl;
                files.push_back(new_path);
            }
        }
    }
    return files;
}

std::string getFileName(const std::string& s) {
    char sep = '/';

    #ifdef _WIN32
    sep = '\\';
    #endif

    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return(s.substr(i+1, s.length() - i));
    }

    return("");
}

std::string getPathOnly(const std::string& s) {
    char sep = '/';

    #ifdef _WIN32
    sep = '\\';
    #endif

    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return(s.substr(0, i+1));
    }

    return("");
}

std::ifstream::pos_type filesize(const char* filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

std::string encrypt(const std::string& original)
{
    char key = 'x';
    std::string encrypted;
    for(int i = 0; i < original.size(); ++i)
    {
        encrypted += original[i] ^ (int(key) + i) % 20;
    }
//    std::cout <<"Original is: " << original << std::endl;
//    std::cout << "Encrypted is: " << encrypted << std::endl;
    return encrypted;
}

std::string decrypt(const std::string& encrypted)
{
    char key = 'x';
    std::string original;
    for(int i = 0; i < encrypted.size(); ++i)
    {
        original += encrypted[i] ^ (int(key) + i) % 20;
    }
//    std::cout << "Encrypted is: " << encrypted << std::endl;
//    std::cout <<"Original is: " << original << std::endl;
    return original;
}

bool compareUsers(const std::string& username, const std::string& password)
{
    std::ifstream input;

    input.open(SIGNED_UP_FILENAME);

    if(input.is_open())
    {

        std::string name_temp;
        std::string password_temp;
        input>>name_temp;

        while(!input.eof())
        {
            input>>password_temp;

            if(username == decrypt(name_temp) && password == decrypt(password_temp))
            {
                input.close();
                return true;
            }

            input>>name_temp;
        }

        input.close();
    }
    return false;
}
