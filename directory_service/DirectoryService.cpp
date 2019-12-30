//
//  DirectoryService.cpp
//  Distributed Project
//
//  Created by Mohamed on 11/7/18.
//  Copyright © 2018 Mohamed Tawfik. All rights reserved.
//


#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include <mutex>

#include "InterfaceDefintion.h"
#include "DirectoryService.hpp"
#include "ErrorHandler.hpp"
#include "LocalDirectory.hpp"
#include "Server.hpp"
#include "API.hpp"

bool DirectoryService::isUsernameAvailable(const std::string& new_username) {
	std::cout << "The username requested is " << new_username << std::endl;
	bool cond1 = new_username != DIRECTORY_SERVICE_USERNAME;
	bool cond2 = credentials.find(new_username) == credentials.end();
	std::cout << "Condition 1 is " << cond1 << std::endl;
	std::cout << "Condition 2 is " << cond2 << std::endl; 
    return (cond1 && cond2);
}
bool DirectoryService::isCredentialsCorrect(const std::string& username, const std::string& password) {
    if(!isUsernameAvailable(username)) {
        return(credentials.at(username) == password);
    } else {
        return false;
    }
}

bool DirectoryService::isStringLegal(const std::string& string_to_check) {
    if(string_to_check.length() == 0) {
        return false;
    }
    for(unsigned int i = 0; i < string_to_check.length(); i++) {
        if(std::isspace(string_to_check[i]) || string_to_check[i] == '_') {
            return false;
        }
    }
    return true;
}

DirectoryService::DirectoryService() {
    readFromFiles();
}

DirectoryService::~DirectoryService() {
    saveToFiles();
}

bool DirectoryService::signUp(const std::string& new_username, const std::string& password, std::string& reply_msg) {
	// idnetify the mutexes this function uses
	std::lock(mutex_credentials_map, mutex_images_map, mutex_changes_map, mutex_announced_map);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_images(mutex_images_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_changes(mutex_changes_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_announced(mutex_announced_map, std::adopt_lock);


    if(!isStringLegal(new_username)) {
        reply_msg = "Invalid string for username";
        return false;
    } else if(!isStringLegal(password)) {
        reply_msg = "Invalid string for password";
        return false;
    } else if(!isUsernameAvailable(new_username)) {
        reply_msg = "This username is REALLY already in use";
        return false;
    } else {
        credentials[new_username] = password;

        std::vector<std::string> empty_announced_images;
        empty_announced_images.resize(0);
        announced_images[new_username] = empty_announced_images;
        
        std::vector<PendingQuotaChange> empty_pending_requests;
        empty_pending_requests.resize(0);
        pending_quota_changes[new_username] = empty_pending_requests;

        std::vector<ImageReference> new_available_images;
        new_available_images.resize(0);

        for (std::map<std::string, std::vector<std::string> >::const_iterator it = announced_images.begin(); it != announced_images.end(); ++it) {
            std::string owner_username = it->first;
            for(unsigned int i = 0; i < it->second.size(); i++) {
                ImageReference new_image_ref;

                new_image_ref.owner_username = owner_username;
                new_image_ref.image_name = it->second[i];

                new_available_images.push_back(new_image_ref);
            }
        }

        
        available_images[new_username] = new_available_images;
        
        reply_msg = "Thank you for signing up to our system. We hope you enjoy it.";
        return true;
    }
}

bool DirectoryService::login(const std::string& username, const std::string& password) {
	// idnetify the mutexes this function uses
    mutex_credentials_map.lock();
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);

    if(isCredentialsCorrect(username, password)) {
        //        lastKnown_addresses[username] = user_server_address;
        return true;
    } else {
        return false;
    }
}

void DirectoryService::announceAvailability(const std::string& username, const std::string& password, InternetAddress user_server_address) {
    // idnetify the mutexes this function uses
	std::lock(mutex_credentials_map, mutex_addresses_map);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_addresses(mutex_addresses_map, std::adopt_lock);

    if(isCredentialsCorrect(username, password)) {
        lastKnown_addresses[username] = user_server_address;

		for (std::map<std::string, InternetAddress>::iterator it = lastKnown_addresses.begin(); it != lastKnown_addresses.end(); ++it) {
	        std::string other_username = it->first;
	        if(other_username == username) {
	            continue;
	        } else {
	            if(it->second == user_server_address) {
	            	it->second.ip = LocalDirectory::IP_fromString(JUNK_IP_ADDRESS);
	            }
	        }
        
    	}
    }
}

bool DirectoryService::changePassword(const std::string& username, const std::string& old_password, const std::string& new_password) {
    // idnetify the mutexes this function uses
    mutex_credentials_map.lock();
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);

    if(isCredentialsCorrect(username, old_password)) {
        credentials[username] = new_password;
        return true;
    } else {
        return false;
    }
}

bool DirectoryService::getAddressOfUser(const std::string& my_username, const std::string& my_password, const std::string& required_username, InternetAddress& address) {
    // idnetify the mutexes this function uses
    std::lock(mutex_credentials_map, mutex_addresses_map);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_addresses(mutex_addresses_map, std::adopt_lock);

    address = LocalDirectory::getEmptyAddress();
    
    if(isCredentialsCorrect(my_username, my_password)) {
        try {
            address = lastKnown_addresses.at(required_username);
            return true;
        } catch(const std::out_of_range& o) {
            address = LocalDirectory::getEmptyAddress();
            ErrorHandler::displayWarning(o.what());
            return false;
        }
        return false;
    } else {
        return false;
    }
}

void DirectoryService::offerService(const std::string& owner_username, const std::string& owner_password, const std::string& image_name, const std::vector<ImageView>& arr) {
    // idnetify the mutexes this function uses
    std::lock(mutex_credentials_map, mutex_images_map);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_images(mutex_images_map, std::adopt_lock);

    if(!isCredentialsCorrect(owner_username, owner_password)) {
        // Invalid credentials
        ErrorHandler::displayWarning("Incorrect credentials. Possible attack on system");
        return;
    }
    for(unsigned int i = 0; i < arr.size(); i++) {
        bool done = false;
        ImageReference new_img_ref;
        new_img_ref.image_name = image_name;
        new_img_ref.owner_username = owner_username;
        for(unsigned int j = 0; j < available_images[arr[i].username].size() && !done; j++) {
            if(available_images[arr[i].username][j] == new_img_ref) {
                if(!arr[i].canView) {
                    available_images[arr[i].username].erase(available_images[arr[i].username].begin() + j);
                }
                done = true;
            }
        }
        if(!done && arr[i].canView) {
            if(isUsernameAvailable(arr[i].username)) {
                // Invalid shared-with username
                return;
            } else {
                available_images[arr[i].username].push_back(new_img_ref);
            }
        }
    }
}
void DirectoryService::getAvailableServices(const std::string& my_username, const std::string& my_password, std::vector<ImageReference>& offerings) {
    // idnetify the mutexes this function uses
    std::lock(mutex_credentials_map, mutex_images_map, mutex_announced_map);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_images(mutex_images_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_announced(mutex_announced_map, std::adopt_lock);


    if(!isCredentialsCorrect(my_username, my_password)) {
        // Invalid credentials
        ErrorHandler::displayWarning("Incorrect credentials. Possible attack on system");
        return;
    }
    try {
        // offerings = available_images.at(my_username);
        offerings.clear();
        for (std::map<std::string, std::vector<std::string> >::const_iterator it = announced_images.begin(); it != announced_images.end(); ++it) {
            std::string owner_username = it->first;
            if(owner_username == my_username) {
                continue;
            }
            for(unsigned int i = 0; i < it->second.size(); i++) {
                ImageReference new_img_ref;
                new_img_ref.owner_username = owner_username;
                new_img_ref.image_name = it->second[i];
                offerings.push_back(new_img_ref);
            }
        }


    } catch(const std::out_of_range& o) {
        offerings.clear();
        ErrorHandler::displayWarning(o.what());
    }
}
void DirectoryService::getUsernames(const std::string& my_username, const std::string& my_password, std::vector<std::string>& usernames) {
    // idnetify the mutexes this function uses
    mutex_credentials_map.lock();
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);

    usernames.clear();
    if(!isCredentialsCorrect(my_username, my_password)) {
        // Invalid credentials
        ErrorHandler::displayWarning("Incorrect credentials. Possible attack on system");
        return;
    }
    for (std::map<std::string,std::string>::const_iterator it = credentials.begin(); it != credentials.end(); ++it) {
        usernames.push_back(it->first);
    }
}

void DirectoryService::submitPendingQuotaChangeRequest(const std::string& my_username, const std::string& my_password, const std::string& shared_with_username, PendingQuotaChange request_from_owner) {
    // idnetify the mutexes this function uses
    std::lock(mutex_credentials_map, mutex_changes_map);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_changes(mutex_changes_map, std::adopt_lock);

    if(!isCredentialsCorrect(my_username, my_password)) {
        // Invalid credentials
        ErrorHandler::displayWarning("Incorrect credentials. Possible attack on system");
        return;
    }
    if(isUsernameAvailable(shared_with_username)) {
        // Invalid shared-with username
        return;
    }
    pending_quota_changes[shared_with_username].push_back(request_from_owner);
}
void DirectoryService::getPendingQuotaChangeRequests(const std::string& my_username, const std::string& my_password,std::vector<PendingQuotaChange>& requests) {
    // idnetify the mutexes this function uses
    std::lock(mutex_credentials_map, mutex_changes_map);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_changes(mutex_changes_map, std::adopt_lock);

    requests.clear();
    if(!isCredentialsCorrect(my_username, my_password)) {
        // Invalid credentials
        ErrorHandler::displayWarning("Incorrect credentials. Possible attack on system");
        return;
    }
    try {
        requests = pending_quota_changes.at(my_username);
    } catch(const std::out_of_range& o) {
        ErrorHandler::displayWarning(o.what());
    }
}
void DirectoryService::removePendingQuotaChangeRequests(const std::string& username_to_remove, const std::string& password) {
    // idnetify the mutexes this function uses
    std::lock(mutex_credentials_map, mutex_changes_map);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_changes(mutex_changes_map, std::adopt_lock);

    if(!isCredentialsCorrect(username_to_remove, password)) {
        // Invalid shared-with username
        return;
    }
    pending_quota_changes.at(username_to_remove).clear();
}

void DirectoryService::announceNewImage(const std::string& username, const std::string& password, const std::string& image_name) {
    std::lock(mutex_credentials_map, mutex_images_map, mutex_announced_map);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_announced(mutex_announced_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_images(mutex_images_map, std::adopt_lock);

     if(!isCredentialsCorrect(username, password)) {
        // Invalid shared-with username
        return;
    }
    announced_images.at(username).push_back(image_name);

    // for(unsigned int i = 0; i < image_name.length(); i++) {
    //     if(image_name[i] == ' ') {
    //         std::cout << "WARNING **********************************************************************" << std::endl;
    //         std::cout << "The image name contains a space" << std::endl;
    //     }
    //      if(image_name[i] == '\t') {
    //         std::cout << "WARNING **********************************************************************" << std::endl;
    //         std::cout << "The image name contains a tab" << std::endl;
    //     }
    //      if(image_name[i] == '\r') {
    //         std::cout << "WARNING **********************************************************************" << std::endl;
    //         std::cout << "The image name contains a carriage return" << std::endl;
    //     }
    //      if(image_name[i] == '\n') {
    //         std::cout << "WARNING **********************************************************************" << std::endl;
    //         std::cout << "The image name contains an end line" << std::endl;
    //     }
        
    // }

    ImageReference new_img_ref;

    new_img_ref.owner_username = username;
    new_img_ref.image_name = image_name;

    for (std::map<std::string, std::vector<ImageReference> >::iterator it = available_images.begin(); it != available_images.end(); ++it) {
        std::string shared_with_username = it->first;
        if(shared_with_username == username) {
            continue;
        } else {
            it->second.push_back(new_img_ref);
        }
    }

}

void DirectoryService::saveLastKnownAddresses() {
    std::ofstream addresses_file;
    addresses_file.open(Last_Known_Addresses_Filename);
    if(addresses_file.is_open()) {
        addresses_file << lastKnown_addresses.size() << std::endl;
        for (std::map<std::string, InternetAddress>::const_iterator it = lastKnown_addresses.begin(); it != lastKnown_addresses.end(); ++it) {
            addresses_file << it->first << " " << LocalDirectory::IP_toString(it->second.ip) << " " << it->second.port << std::endl;
        }
        addresses_file.close();
    } else {
        ErrorHandler::displayWarning("Unable to save the last known ip addresses");
    }
}
void DirectoryService::saveCredentials() {
    std::ofstream credentials_file;
    credentials_file.open(Credentials_Filename);
    if(credentials_file.is_open()) {
        credentials_file << credentials.size() << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = credentials.begin(); it != credentials.end(); ++it) {
            credentials_file << it->first << " " << it->second << std::endl;
        }
        credentials_file.close();
    } else {
        ErrorHandler::displayWarning("Unable to save the credentials");
    }
}
void DirectoryService::saveAvailableImages() {
    std::ofstream offered_images_file;
    offered_images_file.open(Available_Images_Filename);
    if(offered_images_file.is_open()) {
        offered_images_file << available_images.size() << std::endl;
        for (std::map<std::string, std::vector<ImageReference> >::const_iterator it = available_images.begin(); it != available_images.end(); ++it) {
            offered_images_file << it->first << " " << it->second.size() << std::endl;
            for(unsigned int i = 0; i < it->second.size(); i++) {
                //offered_images_file << it->second[i].owner_username << " " << it->second[i].image_name << std::endl;
                offered_images_file << it->second[i].owner_username << " ";
                offered_images_file << it->second[i].image_name.length();
                for(unsigned int j = 0; j < it->second[i].image_name.length(); j++) {
                    offered_images_file.put(it->second[i].image_name[j]);
                }
                offered_images_file << std::endl;
            }
        }
        offered_images_file.close();
    } else {
        ErrorHandler::displayWarning("Unable to save the offered images");
    }
}
void DirectoryService::savePendingQuotaChanges() {
    std::ofstream pending_changes_file;
    pending_changes_file.open(Pending_Quota_Changes_Filename);
    if(pending_changes_file.is_open()) {
        pending_changes_file << pending_quota_changes.size() << std::endl;
        for (std::map<std::string, std::vector<PendingQuotaChange> >::const_iterator it = pending_quota_changes.begin(); it != pending_quota_changes.end(); ++it) {
            pending_changes_file << it->first << " " << it->second.size() << std::endl;
            for(unsigned int i = 0; i < it->second.size(); i++) {
                pending_changes_file << it->second[i].remote_img_ref.img_ref.owner_username << " " << it->second[i].remote_img_ref.img_ref.image_name << " " << it->second[i].remote_img_ref.shared_with_username << " " << it->second[i].new_quota << std::endl;
            }
        }
        pending_changes_file.close();
    } else {
        ErrorHandler::displayWarning("Unable to save the pending quota changes");
    }
}
void DirectoryService::saveAnnouncedImages() {
    std::ofstream announced_images_file;
    announced_images_file.open(Announced_Images_Filename);
    if(announced_images_file.is_open()) {
        announced_images_file << announced_images.size() << std::endl;
        for (std::map<std::string, std::vector<std::string> >::const_iterator it = announced_images.begin(); it != announced_images.end(); ++it) {
            announced_images_file << it->first << " " << it->second.size() << std::endl;
            for(unsigned int i = 0; i < it->second.size(); i++) {
                announced_images_file << it->second[i].length() << " ";

                 
                std::cout << "Saving the announced image" << std::endl;
                std::cout << "The image length is " << it->second[i].length() << std::endl;

                for(unsigned int j = 0; j < it->second[i].length(); j++) {
                    announced_images_file.put(it->second[i][j]);
                }
                announced_images_file << std::endl;
            }
        }
        announced_images_file.close();
    } else {
        ErrorHandler::displayWarning("Unable to save the announced images");
    }
}

void DirectoryService::saveToFiles() {
	std::lock(mutex_credentials_map, mutex_addresses_map, mutex_images_map, mutex_changes_map, mutex_announced_map, 
		mutex_credentials_file, mutex_addresses_file, mutex_images_file, mutex_changes_file, mutex_announced_file);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_addresses(mutex_addresses_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_images(mutex_images_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_changes(mutex_changes_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_announced(mutex_announced_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_credentials_file(mutex_credentials_file, std::adopt_lock);
    std::lock_guard<std::mutex> lk_addresses_file(mutex_addresses_file, std::adopt_lock);
    std::lock_guard<std::mutex> lk_images_file(mutex_images_file, std::adopt_lock);
    std::lock_guard<std::mutex> lk_changes_file(mutex_changes_file, std::adopt_lock);
    std::lock_guard<std::mutex> lk_announced_file(mutex_announced_file, std::adopt_lock);


    saveCredentials();
    saveLastKnownAddresses();
    saveAvailableImages();
    savePendingQuotaChanges();
    saveAnnouncedImages();
}

void DirectoryService::readLastKnownAddresses() {
    lastKnown_addresses.clear();
    std::ifstream addresses_file;
    addresses_file.open(Last_Known_Addresses_Filename);
    if(addresses_file.is_open()) {
        unsigned int addresses_count;
        addresses_file >> addresses_count;
        for(unsigned int i = 0; i < addresses_count; i++) {
            std::string username;
            InternetAddress location;
            std::string ip_string;
            addresses_file >> username >> ip_string >> location.port;
            location.ip = LocalDirectory::IP_fromString(ip_string);
            lastKnown_addresses[username] = location;
        }
        addresses_file.close();
    } else {
        ErrorHandler::displayWarning("Unable to read the last known ip addresses");
    }
}
void DirectoryService::readCredentials() {
    credentials.clear();
    std::ifstream credentials_file;
    credentials_file.open(Credentials_Filename);
    if(credentials_file.is_open()) {
        unsigned int credentials_count;
        credentials_file >> credentials_count;
        for(unsigned int i = 0; i < credentials_count; i++) {
            std::string username;
            std::string password;
            credentials_file >> username >> password;
            credentials[username] = password;
        }
        credentials_file.close();
    } else {
        ErrorHandler::displayWarning("Unable to read the credentials");
    }
}
void DirectoryService::readAvailableImages() {
    available_images.clear();
    std::ifstream offered_images_file;
    offered_images_file.open(Available_Images_Filename);
    if(offered_images_file.is_open()) {
        unsigned int users_count;
        offered_images_file >> users_count;
        for(unsigned int i = 0; i < users_count; i++) {
            std::string shared_with_username;
            unsigned int available_resources_for_username;
            offered_images_file >> shared_with_username >> available_resources_for_username;
            std::vector<ImageReference> available_resources;
            available_resources.clear();
            for(unsigned int j = 0; j < available_resources_for_username; j++) {
                ImageReference new_image_ref;
                offered_images_file >> new_image_ref.owner_username;
                uint32_t image_length = 0;
                offered_images_file >> image_length;

                char temp = 0;
                offered_images_file.get(temp); // the space

                //new_image_ref.image_name = "";

                for(unsigned int k = 0; k < image_length; k++) {
                    offered_images_file.get(temp);

                    new_image_ref.image_name.append(1, temp);
                }

                // offered_images_file.get(temp);

                available_resources.push_back(new_image_ref);
            }

            available_images[shared_with_username] = available_resources;
        }
        offered_images_file.close();
    } else {
        ErrorHandler::displayWarning("Unable to read the available resources");
    }
}
void DirectoryService::readPendingQuotaChanges() {
    pending_quota_changes.clear();
    std::ifstream pending_changes_file;
    pending_changes_file.open(Pending_Quota_Changes_Filename);
    if(pending_changes_file.is_open()) {
        unsigned int users_count;
        pending_changes_file >> users_count;
        for(unsigned int i = 0; i < users_count; i++) {
            std::string target_username;
            unsigned int pending_changes_num;
            pending_changes_file >> target_username >> pending_changes_num;
            std::vector<PendingQuotaChange> pending_changes;
            pending_changes.clear();
            for(unsigned int j = 0; j < pending_changes_num; j++) {
                PendingQuotaChange new_change;
                pending_changes_file >> new_change.remote_img_ref.img_ref.owner_username >> new_change.remote_img_ref.img_ref.image_name >> new_change.remote_img_ref.shared_with_username >> new_change.new_quota;
                pending_changes.push_back(new_change);
            }
            pending_quota_changes[target_username] = pending_changes;
        }
        pending_changes_file.close();
    } else {
        ErrorHandler::displayWarning("Unable to read the pending quota changes");
    }
}

void DirectoryService::readAnnouncedImages() {
    announced_images.clear();
    std::ifstream announced_images_file;
    announced_images_file.open(Announced_Images_Filename);
    if(announced_images_file.is_open()) {
        unsigned int users_count;
        announced_images_file >> users_count;
        std::cout << "There are " << users_count << " users" << std::endl;
        for(unsigned int i = 0; i < users_count; i++) {
            std::string owner_username;
            unsigned int annonuced_images_of_username;
            announced_images_file >> owner_username >> annonuced_images_of_username;
            std::vector<std::string> announced_resources;
            announced_resources.clear();

            std::cout << "The owner username is " << owner_username << std::endl;
            std::cout << "This user has " << annonuced_images_of_username << " images" << std::endl;
            for(unsigned int j = 0; j < annonuced_images_of_username; j++) {
                std::cout << "Reading the announced image number " << j << std::endl;

                std::string new_image = "";

                uint32_t image_length = 0;
                announced_images_file >> image_length;

                std::cout << "The image length is " << image_length << std::endl;

                char temp = 0;
                announced_images_file.get(temp); // the space

                std::cout << "The first temp character is $" << temp << "$" << std::endl;

                for(unsigned int k = 0; k < image_length; k++) {
                    announced_images_file.get(temp);
                    new_image.append(1, temp);
                }

                std::cout << "I have read " << new_image.length() << " characters" << std::endl;

                // announced_images_file.get(temp);


                announced_resources.push_back(new_image);
            }
            std::cout << "The owner username in readAnnouncedImages is " << owner_username << std::endl;
            announced_images[owner_username] = announced_resources;
        }
        announced_images_file.close();
    } else {
        ErrorHandler::displayWarning("Unable to read the announced resources");
    }
}

void DirectoryService::readFromFiles() {
	std::lock(mutex_credentials_map, mutex_addresses_map, mutex_images_map, mutex_changes_map, mutex_announced_map, 
        mutex_credentials_file, mutex_addresses_file, mutex_images_file, mutex_changes_file, mutex_announced_file);
    std::lock_guard<std::mutex> lk_credentials(mutex_credentials_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_addresses(mutex_addresses_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_images(mutex_images_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_changes(mutex_changes_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_announced(mutex_announced_map, std::adopt_lock);
    std::lock_guard<std::mutex> lk_credentials_file(mutex_credentials_file, std::adopt_lock);
    std::lock_guard<std::mutex> lk_addresses_file(mutex_addresses_file, std::adopt_lock);
    std::lock_guard<std::mutex> lk_images_file(mutex_images_file, std::adopt_lock);
    std::lock_guard<std::mutex> lk_changes_file(mutex_changes_file, std::adopt_lock);
    std::lock_guard<std::mutex> lk_announced_file(mutex_announced_file, std::adopt_lock);
    
    readLastKnownAddresses();
    readCredentials();
    readAvailableImages();
    readPendingQuotaChanges();
    readAnnouncedImages();
}

/*
void directory_service_main() {
    
    DirectoryService * directory_service = new DirectoryService();
    
    Server * server = new Server(LocalDirectory::GetPrimaryIp(), DIRECTORY_PORT);
    
    std::cout << "--> I am the directory service. Please notify others with my ip address and port number." << std::endl;
    std::cout << "--> My IP address is: " << LocalDirectory::IP_toString(LocalDirectory::GetPrimaryIp()) << std::endl;
    std::cout << "--> My port number is: " << DIRECTORY_PORT << std::endl;
    std::cout << "--> Waiting to receive requests" << std::endl;
    
    
    while(directory_service != NULL) {
        Message * request = server->receiveRequest();
        request->print();
        Message * reply;
        
        switch (request->getOperation()) {
                // Requests for testing
            case OperationID::RESERVED:
                ErrorHandler::displayWarning("Using reserved operation id");
                reply = request->createEmptyReply();
                break;
            case OperationID::EMPTY:
                ErrorHandler::displayWarning("Using empty operation id");
                reply = request->createEmptyReply();
                break;
                // Requests of directory service
            case OperationID::SIGNUP:
                reply = hanle_sign_up_reqesut(directory_service, request);
                break;
            case OperationID::LOGIN:
                reply = handle_login_request(directory_service, request);
                break;
            case OperationID::HI_SERVER:
                reply = handle_announce_availability(directory_service, request);
                break;
            case OperationID::GET_ADDRESS_OF_USER:
                reply = handle_map_username_to_ip_request(directory_service, request);
                break;
            case OperationID::CHANGE_PASSWORD:
                reply = handle_change_password_request(directory_service, request);
                break;
            case OperationID::OFFER_SERVICE:
                reply = handle_offer_services_request(directory_service, request);
                break;
            case OperationID::GET_AVAILABLE_SERVICES:
                reply = handle_get_available_servies_request(directory_service, request);
                break;
            case OperationID::GET_USERNAMES:
                reply = handle_get_usernames_request(directory_service, request);
                break;
            case OperationID::SUBMIT_PENDING_QUOTA_CHANGE_REQUEST:
                reply = handle_submit_pending_quota_change_request(directory_service, request);
                break;
            case OperationID::REMOVE_PENDING_QUOTA_CHANGES_REQUEST:
                reply = handle_remove_pending_quota_changes_request(directory_service, request);
                break;
            case OperationID::TURN_OFF_DIRECTORY_SERVICE:
                reply = handle_turn_off_directory_service_request(directory_service, request);
                break;
            default:
                ErrorHandler::displayWarning("Using unknown operation id");
                ErrorHandler::displayWarning("This unknown operation id is " + patch::to_string(request->getOperation()));
                reply = request->createEmptyReply();
                break;
        }
        
        std::cout << "The reply is" << std::endl;
        reply->print();
        server->sendReply(reply);
        
        delete request;
        delete reply;
    }
    
    delete server;
}
*/

Message * hanle_sign_up_reqesut(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for sign up api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    std::string password;
    API::signup_arrayToArguments(args, password);
    std::string reply_msg;
    
    // Handle request
    bool isAccepted = directory->signUp(request->getSenderUsername(), password, reply_msg);
    
    // Marshall the results
    uint32_t reply_size;
    char * results = API::signup_resultsToArray(isAccepted, reply_msg, reply_size);
    
    // Create reply
    Message * reply = request->createReply(reply_size, results);
    
    delete [] args;
    delete [] results;
    
    return reply;
}
Message * handle_login_request(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for sign up api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    std::string password;
    API::login_arrayToArguments(args, password);
    std::string reply_msg;
    
    // Handle request
    bool isAccepted = directory->login(request->getSenderUsername(), password);
    
    // Marshall the results
    uint32_t reply_size;
    char * results = API::login_resultsToArray(isAccepted, reply_size);
    
    // Create reply
    Message * reply = request->createReply(reply_size, results);
    
    delete [] args;
    delete [] results;
    
    return reply;
}
Message * handle_announce_availability(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for hi server api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    InternetAddress user_service_address;
    std::string password;
    API::hiServer_arrayToArguments(args, user_service_address.ip, user_service_address.port, password);
    
    // Handle request
    directory->announceAvailability(request->getSenderUsername(), password, user_service_address);
    std::vector<PendingQuotaChange> changes;
    directory->getPendingQuotaChangeRequests(request->getSenderUsername(), password, changes);

    std::cout << "The pending changes requests are " << std::endl;

    for(unsigned int i = 0; i < changes.size(); i++) {
    	std::cout << changes[i].remote_img_ref.img_ref.owner_username << " " << changes[i].remote_img_ref.img_ref.image_name << " ";
    	std::cout << changes[i].remote_img_ref.shared_with_username << " " << changes[i].new_quota << std::endl;
    }
    
    // Marshall the results
    uint32_t reply_size;
    char * results = API::hiServer_resultsToArray(changes, reply_size);
    
    // Create reply
    Message * reply = request->createReply(reply_size, results);
    
    delete [] args;
    delete [] results;
    return reply;
}
Message * handle_map_username_to_ip_request(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for mapping a username to ip api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    std::string required_username;
    std::string password;
    API::mapUsernameToAddress_arrayToArguments(args, password, required_username);
    
    // Handle request
    InternetAddress address;
    bool result = directory->getAddressOfUser(request->getSenderUsername(), password, required_username, address);
    
    // Marshall the results
    uint32_t reply_size;
    char * results = API::mapUsernameToAddress_resultsToArray(result, address, reply_size);
    
    // Create reply
    Message * reply = request->createReply(reply_size, results);
    
    delete [] args;
    delete [] results;
    return reply;
}
Message * handle_change_password_request(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for sign up api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    std::string old_password, new_password;
    API::changePassword_arrayToArguments(args, old_password, new_password);
    
    // Handle request
    bool isAccepted = directory->changePassword(request->getSenderUsername(), old_password, new_password);
    
    // Marshall the results
    uint32_t reply_size;
    char * results = API::changePassword_resultsToArray(isAccepted, reply_size);
    
    // Create reply
    Message * reply = request->createReply(reply_size, results);
    
    delete [] args;
    delete [] results;
    
    return reply;
}
Message * handle_get_usernames_request(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for sign up api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    std::string password;
    API::getUsers_arrayToArguments(args, password);
    std::vector<std::string> usernames;
    usernames.resize(0);
    
    // Handle request
    directory->getUsernames(request->getSenderUsername(), password, usernames);
    
    // Marshall the results
    uint32_t reply_size;
    char * results = API::getUsers_resultsToArray(usernames, reply_size);
    
    // Create reply
    Message * reply = request->createReply(reply_size, results);
    
    delete [] args;
    delete [] results;
    
    return reply;
}
Message * handle_offer_services_request(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for sign up api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    std::string password, image_name;
    std::vector<ImageView> arr;
    API::allowUsertoView_arrayToArguments(args, password, image_name, arr);
    
    std::cout << "Now attempting to offer a new service" << std::endl;
    std::cout << "The owner is " << request->getSenderUsername() << std::endl;
    std::cout << "The image is " << image_name << std::endl;
    std::cout << "The array is " << std::endl;
    for(unsigned int i = 0; i < arr.size(); i++) {
        std::cout << arr[i].username << " " << arr[i].canView << std::endl;
    }
    
    // Handle request
    directory->offerService(request->getSenderUsername(), password, image_name, arr);
    
    
    // Create reply
    Message * reply = request->createEmptyReply();
    
    delete [] args;
    
    return reply;
}
Message * handle_get_available_servies_request(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for sign up api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    std::string password;
    API::retreiveImages_arrayToArguments(args, password);
    std::vector<ImageReference> offerings;
    offerings.resize(0);
    
    // Handle request
    directory->getAvailableServices(request->getSenderUsername(), password, offerings);
    
    // Marshall the results
    uint32_t reply_size;
    char * results = API::retreiveImages_resultsToArray(offerings, reply_size);
    
    // Create reply
    Message * reply = request->createReply(reply_size, results);
    
    delete [] args;
    delete [] results;
    
    return reply;
}
Message * handle_submit_pending_quota_change_request(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for sign up api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    std::string password;
    PendingQuotaChange new_change;
    API::submitPendingQuotaChange_arrayToArguments(args, password, new_change);
    
    // Handle request
    directory->submitPendingQuotaChangeRequest(request->getSenderUsername(), password, new_change.remote_img_ref.shared_with_username, new_change);
    
    // Marshall the results
    
    // Create reply
    Message * reply = request->createEmptyReply();
    
    delete [] args;
    
    return reply;
}
Message * handle_remove_pending_quota_changes_request(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for remove pending quota changes api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    std::string password;
    API::removePendingQuotaChanges_arrayToArguments(args, password);
    
    // Handle request
    directory->removePendingQuotaChangeRequests(request->getSenderUsername(), password);
    
    // Marshall the results
    
    // Create reply
    Message * reply = request->createEmptyReply();
    
    delete [] args;
    
    return reply;
}
Message * handle_turn_off_directory_service_request(DirectoryService *& directory, Message * request) {
    std::cout << "I'm turning off" << std::endl;
    Message * reply = request->createEmptyReply();
    delete directory;
    directory = NULL;
    return reply;
}
Message * handle_announce_image(DirectoryService *& directory, Message * request) {
    // Retreive args array
    char * args;
    uint32_t args_size = request->getArgumentsSize();
    args = new char [args_size];
    ErrorHandler::checkPointer(args, "Memory allocation for announce image api unmarshalling failed");
    request->getArguments(args, args_size);
    
    // Unmarshall the arguemnts
    std::string password, image_name;
    API::announceImage_arrayToArguments(args, password, image_name);
    
    // Handle request
    directory->announceNewImage(request->getSenderUsername(), password, image_name);
    
    // Create reply
    Message * reply = request->createEmptyReply();

    
    delete [] args;
    
    return reply;
}
