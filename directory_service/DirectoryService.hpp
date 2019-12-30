//
//  DirectoryService.hpp
//  Distributed Project
//
//  Created by Mohamed on 11/7/18.
//  Copyright © 2018 Mohamed Tawfik. All rights reserved.
//

#ifndef DirectoryService_hpp
#define DirectoryService_hpp

#include <vector>
#include <map>
#include <string>

#include <mutex> 

#include "InterfaceDefintion.h"
#include "Message.hpp"

#define Last_Known_Addresses_Filename "last_known_addresses.txt"
#define Credentials_Filename "credentials.txt"
#define Available_Images_Filename "available_images.txt"
#define Pending_Quota_Changes_Filename "pending_quota_changes.txt"
#define Announced_Images_Filename "announced_images.txt"

#define JUNK_IP_ADDRESS "8.8.8.8"

class DirectoryService {
protected:
    // Maps a username to their correct password
    std::mutex mutex_credentials_map;
    std::map<std::string, std::string> credentials;

    // Maps a username to their last known location
    std::mutex mutex_addresses_map;
    std::map<std::string, InternetAddress> lastKnown_addresses;
    
    // Maps a username to the list of available images offered to them by other people
    std::mutex mutex_images_map;
    std::map<std::string, std::vector<ImageReference> > available_images;
    
    // Maps a username to the list of pending requests of quota changes to be pushed asap
    std::mutex mutex_changes_map;
    std::map<std::string, std::vector<PendingQuotaChange> > pending_quota_changes;

    std::mutex mutex_announced_map;
    std::map<std::string, std::vector<std::string> > announced_images;
    
    bool isUsernameAvailable(const std::string& new_username);
    bool isCredentialsCorrect(const std::string& username, const std::string& password);
    
    static bool isStringLegal(const std::string& string_to_check);

    std::mutex mutex_credentials_file;
    void saveCredentials();
    void readCredentials();
    
    std::mutex mutex_addresses_file;
    void saveLastKnownAddresses();
    void readLastKnownAddresses();

    std::mutex mutex_images_file;
    void saveAvailableImages();
    void readAvailableImages();
    
    std::mutex mutex_changes_file;
    void savePendingQuotaChanges();
    void readPendingQuotaChanges();

    std::mutex mutex_announced_file;
    void saveAnnouncedImages();
    void readAnnouncedImages();
    
public:
    DirectoryService();
    ~DirectoryService();
    
    bool signUp(const std::string& new_username, const std::string& password, std::string& reply_msg);
    bool login(const std::string& username, const std::string& password);
    void announceAvailability(const std::string& username, const std::string& password, InternetAddress user_server_address);
    bool changePassword(const std::string& username, const std::string& old_password, const std::string& new_password);

    bool getAddressOfUser(const std::string& my_username, const std::string& my_password, const std::string& required_username, InternetAddress& address);
    
    void offerService(const std::string& owner_username, const std::string& owner_password, const std::string& image_name, const std::vector<ImageView>& arr);
    
    void getAvailableServices(const std::string& my_username, const std::string& my_password, std::vector<ImageReference>& offerings);
    void getUsernames(const std::string& my_username, const std::string& my_password, std::vector<std::string>& usernames);
    
    void submitPendingQuotaChangeRequest(const std::string& my_username, const std::string& my_password, const std::string& shared_with_username, PendingQuotaChange request_from_owner);
    void getPendingQuotaChangeRequests(const std::string& my_username, const std::string& my_password, std::vector<PendingQuotaChange>& requests);
    
    void removePendingQuotaChangeRequests(const std::string& username_to_remove, const std::string& password);

    void announceNewImage(const std::string& owner_username, const std::string& password, const std::string& image_name);
    
    void saveToFiles();
    void readFromFiles();
};

// void directory_service_main();

Message * hanle_sign_up_reqesut(DirectoryService *& directory, Message * request);
Message * handle_login_request(DirectoryService *& directory, Message * request);
Message * handle_announce_availability(DirectoryService *& directory, Message * request);
Message * handle_map_username_to_ip_request(DirectoryService *& directory, Message * request);
Message * handle_change_password_request(DirectoryService *& directory, Message * request);
Message * handle_get_usernames_request(DirectoryService *& directory, Message * request);
Message * handle_offer_services_request(DirectoryService *& directory, Message * request);
Message * handle_get_available_servies_request(DirectoryService *& directory, Message * request);
Message * handle_submit_pending_quota_change_request(DirectoryService *& directory, Message * request);
Message * handle_remove_pending_quota_changes_request(DirectoryService *& directory, Message * request);
Message * handle_turn_off_directory_service_request(DirectoryService *& directory, Message * request);
Message * handle_announce_image(DirectoryService *& directory, Message * request);

#endif /* DirectoryService_hpp */
