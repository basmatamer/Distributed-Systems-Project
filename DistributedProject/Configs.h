#ifndef CONFIGS_H
#define CONFIGS_H

#include <string>
#include <vector>
#include <fstream>
#include <QWidget>
#include <map>
#include "InterfaceDefintion.h"
#include <mutex>

struct ImageReferenceMap {


    std::string owner_username;
    std::string image_name;

    bool operator<(const ImageReferenceMap& other) const{
        if(owner_username < other.owner_username) {
            return true;
        } else {
            return image_name < other.image_name;
        }
    }
};

extern std::string home_directory;
extern std::string user_username_string;
//extern std::map<ImageReferenceMap, std::mutex*> locks_on_images;
extern bool has_internet;

// to unhide
// mv .hiddendir hiddendir
#define application_folder "SharingImages/"

#define app_path (home_directory + application_folder)
#define defpath1 (app_path + user_username_string + "/")
#define files_cache_path (defpath1 + "filesCache/")
#define owned_resources_path (defpath1 + "ownedResources/")
#define icons_path (defpath1 + "icons/")
#define owned_images_path (owned_resources_path + "Images/")
#define shared_resources_path (defpath1 + "sharedResources/")
#define sharedicons_path (defpath1 + "icons/")
#define shared_images_path (shared_resources_path + "Images/")

#define DEFAULT_IMAGE_NAME "def.jpg"

void attemptToLogout(QWidget * from_window);

void setHomeDirectory();
bool checkDirectoryExists(const std::string& path);
bool checkAppPath();
bool checkDefImgPath();
void createFolder(const std::string& path_to_folder_to_create);
void createUserFolder();
std::vector<std::string> GetDirectoryFiles(const std::string& dir);
std::string getFileName(const std::string& s);
std::string getPathOnly(const std::string& s);
std::string encrypt(const std::string&);
std::string decrypt(const std::string&);
bool compareUsers(const std::string&, const std::string&);
std::ifstream::pos_type filesize(const char* filename);

#endif // CONFIGS_H
