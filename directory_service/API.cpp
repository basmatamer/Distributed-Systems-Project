#include "API.hpp"
#include "ErrorHandler.hpp"
#include "Converter.hpp"
#include <stdint.h>
#include <vector>
#include "InterfaceDefintion.h"
#include <iostream>


// Text API
char* API::text_arguemntsToArray(const std::string& text_msg, uint32_t& args_size) {
    uint32_t text_msg_length = (uint32_t)(text_msg.length());

    args_size = 4 + text_msg_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for text API failed");
    char* pointer = args;

    Converter::int_to_chars(text_msg_length, pointer);
    pointer += 4;

    Converter::string_to_chars(text_msg, pointer);
    pointer += text_msg_length;

    return args;
}
void API::text_arrayToArguments(char* args , std::string& text_msg) {
    char * pointer = args;

    uint32_t text_msg_length = Converter::chars_to_int(pointer);
    pointer += 4;

    text_msg = Converter::chars_to_string(text_msg_length, pointer);
    pointer += text_msg_length;
}
char* API::text_resultsToArray(const std::string& reply_msg, uint32_t& args_size) {
    uint32_t reply_msg_length = uint32_t(reply_msg.length());
    args_size = 4 + reply_msg_length;

    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating results for text API failed");
    char * pointer = args;

    Converter::int_to_chars(reply_msg_length, pointer);
    pointer += 4;

    Converter::string_to_chars(reply_msg, pointer);
    pointer += reply_msg_length;

    return args;
}
void API::text_arrayToResults(char* args , std::string& reply_msg) {
    char * pointer = args;

    uint32_t reply_msg_length = Converter::chars_to_int(pointer);
    pointer += 4;

    reply_msg = Converter::chars_to_string(reply_msg_length, pointer);
    pointer += reply_msg_length;
}

// Signup API
char* API::signup_argumentsToArray(const std::string& password, uint32_t& args_size) {
    uint32_t password_length = uint32_t(password.length());

    args_size = 4 + password_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for signup API failed");
    char * pointer = args;

    Converter::int_to_chars(password_length, pointer);
    pointer += 4;

    Converter::string_to_chars(password, pointer);
    pointer += password_length;

    return args;
}
void API::signup_arrayToArguments(char* args, std::string& password) {
    login_arrayToArguments(args, password);
}
char* API::signup_resultsToArray(bool isAccepted, const std::string& text, uint32_t& args_size) {
    uint32_t text_length = (uint32_t)(text.length());
    args_size = 5 + text_length;

    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating results for signup API failed");
    char * pointer = args;

    Converter::bool_to_chars(isAccepted, pointer);
    pointer += 1;

    Converter::int_to_chars(text_length, pointer);
    pointer += 4;

    Converter::string_to_chars(text, pointer);
    pointer += text_length;

    return args;
}
void API::signup_arrayToResults(char* args , bool& isAccepted, std::string& text) {
    char * pointer = args;

    isAccepted = Converter::chars_to_bool(pointer);
    pointer += 1;

    uint32_t text_length = Converter::chars_to_int(pointer);
    pointer += 4;

    text = Converter::chars_to_string(text_length, pointer);
    pointer += text_length;
}

// Login API
char* API::login_argumentsToArray(const std::string& password, uint32_t& args_size) {
    // Convention is Byte no. 0 till 3 is for username_length, Byte no.4 till size of username is for username, then size of password and password itself
    uint32_t password_length = uint32_t(password.length());

    args_size = 4 + password_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for login API failed");

    char * pointer = args;

    Converter::int_to_chars(password_length, pointer);
    pointer += 4;

    Converter::string_to_chars(password, pointer);
    pointer += password_length;

    return args;
}
void API::login_arrayToArguments(char* args, std::string& password) {
    char * pointer = args;

    uint32_t password_length = Converter::chars_to_int(pointer);
    pointer += 4;

    password = Converter::chars_to_string(password_length, pointer);
    pointer += password_length;
}
char* API::login_resultsToArray(bool isAccepted, uint32_t& args_size) {
    args_size = 1;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating results for login API failed");

    char * pointer = args;
    Converter::bool_to_chars(isAccepted, pointer);
    pointer += 1;

    return args;
}
void API::login_arrayToResults(char* args, bool& isAccepted) {
    isAccepted = Converter::chars_to_bool(args);
}

// Hi Server API
char* API::hiServer_argumentsToArray(const std::string& password, uint32_t ipAddress, uint16_t portNumber, uint32_t& args_size) {
    uint32_t password_length = (uint32_t)(password.length());

    args_size = 10 + password_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for Hi server API failed");

    char * pointer = args;

    Converter::int_to_chars(ipAddress, pointer);
    pointer += 4;

    Converter::short_to_chars(portNumber, pointer);
    pointer += 2;

    Converter::int_to_chars(password_length, pointer);
    pointer += 4;

    Converter::string_to_chars(password, pointer);
    pointer += password_length;

    return args;
}
void API::hiServer_arrayToArguments(char* args, std::uint32_t& ipAddress, std::uint16_t& portNumber, std::string& password) {
    char * pointer = args;

    ipAddress = Converter::chars_to_int(pointer);
    pointer += 4;

    portNumber = Converter::chars_to_short(pointer);
    pointer += 2;

    uint32_t password_length = Converter::chars_to_int(pointer);
    pointer += 4;

    password = Converter::chars_to_string(password_length, pointer);
    pointer += password_length;
}
char* API::hiServer_resultsToArray(const std::vector<PendingQuotaChange>& changes, uint32_t& args_size) {
    args_size = 0;

    uint32_t vector_size = (uint32_t)(changes.size());
    args_size += 4;

    for(unsigned j = 0; j < vector_size; j++) {
        args_size += 4 + changes[j].remote_img_ref.img_ref.owner_username.length() + 4 + changes[j].remote_img_ref.img_ref.image_name.length() + 4 + changes[j].remote_img_ref.shared_with_username.length() + 4;
    }

    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for get available images API failed");
    char * pointer = args;

    Converter::int_to_chars(vector_size, pointer);
    pointer += 4;

    for(unsigned j = 0; j < vector_size; j++) {
        uint32_t owner_username_length = (uint32_t)(changes[j].remote_img_ref.img_ref.owner_username.length());
        uint32_t image_length = (uint32_t)(changes[j].remote_img_ref.img_ref.image_name.length());
        uint32_t shared_with_username_length = (uint32_t)(changes[j].remote_img_ref.shared_with_username.length());

        Converter::int_to_chars(owner_username_length, pointer);
        pointer += 4;

        Converter::string_to_chars(changes[j].remote_img_ref.img_ref.owner_username, pointer);
        pointer += owner_username_length;

        Converter::int_to_chars(image_length, pointer);
        pointer += 4;

        Converter::string_to_chars(changes[j].remote_img_ref.img_ref.image_name, pointer);
        pointer += image_length;

        Converter::int_to_chars(shared_with_username_length, pointer);
        pointer += 4;

        Converter::string_to_chars(changes[j].remote_img_ref.shared_with_username, pointer);
        pointer += shared_with_username_length;

        Converter::int_to_chars(changes[j].new_quota, pointer);
        pointer += 4;
    }

    return args;
}
void API::hiServer_arrayToResults(char* args, std::vector<PendingQuotaChange>& changes) {
    char * pointer = args;

    changes.resize(0);

    uint32_t vector_size = Converter::chars_to_int(pointer);
    pointer += 4;

    for(unsigned j = 0; j < vector_size; j++) {
        PendingQuotaChange new_change;

        uint32_t owner_username_length = Converter::chars_to_int(pointer);
        pointer += 4;

        new_change.remote_img_ref.img_ref.owner_username = Converter::chars_to_string(owner_username_length, pointer);
        pointer += owner_username_length;

        uint32_t image_length = Converter::chars_to_int(pointer);
        pointer += 4;

        new_change.remote_img_ref.img_ref.image_name = Converter::chars_to_string(image_length, pointer);
        pointer += image_length;

        uint32_t receiver_username_length = Converter::chars_to_int(pointer);
        pointer += 4;

        new_change.remote_img_ref.shared_with_username = Converter::chars_to_string(receiver_username_length, pointer);
        pointer += receiver_username_length;

        new_change.new_quota = Converter::chars_to_int(pointer);
        pointer += 4;

        changes.push_back(new_change);
    }
}

// Remove pending quota changes API
char* API::removePendingQuotaChanges_argumentsToArray(const std::string& password, uint32_t& args_size) {
    uint32_t password_length = uint32_t(password.length());

    args_size = 4 + password_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for rempve pending quota changes API failed");

    char * pointer = args;

    Converter::int_to_chars((password_length), pointer);
    pointer += 4;

    Converter::string_to_chars(password, pointer);
    pointer += 4;

    return args;
}
void API::removePendingQuotaChanges_arrayToArguments(char* args, std::string& password) {
    char * pointer = args;

    uint32_t password_length = Converter::chars_to_int(pointer);
    pointer += 4;

    password = Converter::chars_to_string(password_length, pointer);
    pointer += password_length;
}

// Change password API
char* API::changePassword_argumentsToArray(const std::string& old_password, const std::string& new_password, uint32_t& args_size) {
    uint32_t old_password_length = (uint32_t) (old_password.length());
    uint32_t new_password_length = (uint32_t) (new_password.length());

    args_size = 4 + old_password_length + 4 + new_password_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating results for change password API failed");
    char* temp = args;

    Converter::int_to_chars(old_password_length, temp);
    temp += 4;

    Converter::string_to_chars(old_password, temp);
    temp += old_password_length;

    Converter::int_to_chars(new_password_length, temp);
    temp += 4;

    Converter::string_to_chars(new_password, temp);
    temp += new_password_length;

    return args;
}
void API::changePassword_arrayToArguments(char* args, std::string& old_password, std::string& new_password) {
    char * temp = args;

    uint32_t old_password_length = Converter::chars_to_int(temp);
    temp += 4;

    old_password = Converter::chars_to_string(old_password_length, temp);
    temp += old_password_length;

    uint32_t new_password_length = Converter::chars_to_int(temp);
    temp += 4;

    new_password = Converter::chars_to_string(new_password_length, temp);
    temp += new_password_length;
}
char* API::changePassword_resultsToArray(bool result, uint32_t& args_size) {
    args_size = 1;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating results for change password API failed");
    char* temp = args;

    Converter::bool_to_chars(result, temp);
    temp += 1;

    return args;
}
void API::changePassword_arrayToResults(char* args, bool& result) {
    char * temp = args;

    result = Converter::chars_to_bool(temp);
    temp += 1;
}

// Submit pending quota change API
char* API::submitPendingQuotaChange_argumentsToArray(const std::string& password, PendingQuotaChange change, uint32_t& args_size) {
    uint32_t password_length = uint32_t(password.length());
    uint32_t owner_username_length = uint32_t(change.remote_img_ref.img_ref.owner_username.length());
    uint32_t image_length = uint32_t(change.remote_img_ref.img_ref.image_name.length());
    uint32_t shared_with_username_length = uint32_t(change.remote_img_ref.shared_with_username.length());

    args_size = 4 + password_length + 4 + owner_username_length + 4 + image_length + 4 + shared_with_username_length + 4 ;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for submit pending quota change API failed");

    char * pointer = args;

    Converter::int_to_chars(password_length, pointer);
    pointer += 4;

    Converter::string_to_chars(password, pointer);
    pointer += password_length;

    Converter::int_to_chars(owner_username_length, pointer);
    pointer += 4;

    Converter::string_to_chars(change.remote_img_ref.img_ref.owner_username, pointer);
    pointer += owner_username_length;

    Converter::int_to_chars(image_length, pointer);
    pointer += 4;

    Converter::string_to_chars(change.remote_img_ref.img_ref.image_name, pointer);
    pointer += image_length;

    Converter::int_to_chars(shared_with_username_length, pointer);
    pointer += 4;

    Converter::string_to_chars(change.remote_img_ref.shared_with_username, pointer);
    pointer += shared_with_username_length;

    Converter::int_to_chars(change.new_quota, pointer);
    pointer += 4;

    return args;
}
void API::submitPendingQuotaChange_arrayToArguments(char * args, std::string& password, PendingQuotaChange& change) {
    uint32_t password_length, sender_username_length, image_length, shared_with_username_length;

    char * pointer = args;

    password_length = Converter::chars_to_int(pointer);
    pointer += 4;

    password = Converter::chars_to_string(password_length, pointer);
    pointer += password_length;

    sender_username_length = Converter::chars_to_int(pointer);
    pointer += 4;

    change.remote_img_ref.img_ref.owner_username = Converter::chars_to_string(sender_username_length, pointer);
    pointer += sender_username_length;

    image_length = Converter::chars_to_int(pointer);
    pointer += 4;

    change.remote_img_ref.img_ref.image_name = Converter::chars_to_string(image_length, pointer);
    pointer += image_length;

    shared_with_username_length = Converter::chars_to_int(pointer);
    pointer += 4;

    change.remote_img_ref.shared_with_username = Converter::chars_to_string(shared_with_username_length, pointer);
    pointer += shared_with_username_length;

    change.new_quota = Converter::chars_to_int(pointer);
}

// Get address of user API
char* API::mapUsernameToAddress_argumentsToArray(const std::string& password, const std::string& required_username, uint32_t& args_size) {
    uint32_t password_length = uint32_t(password.length());
    uint32_t required_username_length = uint32_t(required_username.length());

    args_size = 4 + password_length + 4 + required_username_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for map users to address API failed");

    char * pointer = args;

    Converter::int_to_chars((password_length), pointer);
    pointer += 4;

    Converter::string_to_chars(password, pointer);
    pointer += password_length;

    Converter::int_to_chars((required_username_length), pointer);
    pointer += 4;

    Converter::string_to_chars(required_username, pointer);
    pointer += required_username_length;

    return args;
}
void API::mapUsernameToAddress_arrayToArguments(char * args, std::string& password, std::string& required_username) {
    char * pointer = args;

    uint32_t password_length = Converter::chars_to_int(pointer);
    pointer += 4;

    password = Converter::chars_to_string(password_length, pointer);
    pointer += password_length;

    uint32_t required_username_length = Converter::chars_to_int(pointer);
    pointer += 4;

    required_username = Converter::chars_to_string(required_username_length, pointer);
    pointer += required_username_length;
}
char* API::mapUsernameToAddress_resultsToArray(bool result, InternetAddress address, uint32_t& args_size) {
    args_size = 7;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating results for map users to address API failed");

    char * pointer = args;

    Converter::bool_to_chars(result, pointer);
    pointer += 1;

    Converter::int_to_chars(address.ip, pointer);
    pointer += 4;

    Converter::short_to_chars(address.port, pointer);
    pointer += 2;

    return args;
}
void API::mapUsernameToaddress_arrayToResults(char* args, bool& result, InternetAddress& address) {
    char * pointer = args;

    result = Converter::chars_to_bool(pointer);
    pointer += 1;

    address.ip = Converter::chars_to_int(pointer);
    pointer += 4;

    address.port = Converter::chars_to_short(pointer);
    pointer += 2;
}

// Get Users API
char* API::getUsers_argumentsToArray(const std::string& password, uint32_t& args_size) {
    uint32_t password_length = uint32_t(password.length());

    args_size = 4 + password_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for get users API failed");

    char * pointer = args;

    Converter::int_to_chars((password_length), pointer);
    pointer += 4;
    Converter::string_to_chars(password, pointer);

    return args;
}
void API::getUsers_arrayToArguments(char* args, std::string& password) {
    char * pointer = args;

    uint32_t password_length = Converter::chars_to_int(pointer);
    pointer += 4;

    password = Converter::chars_to_string(password_length, pointer);
    pointer += password_length;
}
char* API::getUsers_resultsToArray(const std::vector<std::string>& usernames, uint32_t& args_size) {
    args_size = 0;

    uint32_t vector_size = (uint32_t)(usernames.size());
    args_size += 4;

    for(unsigned j = 0; j < usernames.size(); j++) {
        args_size += 4 + usernames[j].length();
    }

    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for get users API failed");
    char * pointer = args;

    Converter::int_to_chars(vector_size, pointer);
    pointer += 4;

    for(unsigned j = 0; j < usernames.size(); j++) {
        uint32_t username_length = (uint32_t)(usernames[j].length());
        Converter::int_to_chars(username_length, pointer);
        pointer += 4;

        Converter::string_to_chars(usernames[j], pointer);
        pointer += username_length;
    }

    return args;

}
void API::getUsers_arrayToResults(char* args, std::vector<std::string>& usernames) {
    char * pointer = args;

    usernames.resize(0);

    uint32_t vector_size = Converter::chars_to_int(pointer);
    pointer += 4;

    for(unsigned j = 0; j < vector_size; j++) {
        uint32_t username_length = Converter::chars_to_int(pointer);
        pointer += 4;

        usernames.push_back(Converter::chars_to_string(username_length, pointer));
        pointer += username_length;
    }
}

// Offer service to user API
char* API::allowUsertoView_argumentsToArray(const std::string& password, const std::vector<ImageView>& arr, const std::string& image_name, uint32_t& args_size) {
    args_size = 0;

    args_size += 4;
    uint32_t password_length = (uint32_t)(password.length());
    args_size += password_length;

    args_size += 4;
    uint32_t image_name_length = (uint32_t)(image_name.length());
    args_size += image_name_length;

    args_size += 4;
    for(unsigned j = 0; j < arr.size(); j++) {
        // each entry has 4 bytes for length of username, length of username nafso and 4 bytes for canView
        args_size += 4 + arr[j].username.length() + 1;
    }

    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for requestImage API failed");
    char* temp = args;

    Converter::int_to_chars(password_length, temp);
    temp += 4;

    Converter::string_to_chars(password, temp);
    temp += password_length;

    Converter::int_to_chars(image_name_length, temp);
    temp += 4;

    Converter::string_to_chars(image_name, temp);
    temp += image_name_length;

    Converter::int_to_chars((uint32_t)(arr.size()), temp);
    temp += 4;

    for(unsigned i = 0 ; i < arr.size(); i++) {
        uint32_t username_length = (uint32_t)(arr[i].username.length());

        Converter::int_to_chars(username_length, temp);
        temp += 4;

        Converter::string_to_chars(arr[i].username, temp);
        temp += username_length;

        Converter::bool_to_chars(arr[i].canView, temp);
        temp += 1;
    }

    return args;
}
void API::allowUsertoView_arrayToArguments(char* args, std::string& password, std::string& image_name, std::vector<ImageView>& arr) {
    arr.resize(0);
    char* temp = args;

    uint32_t password_length = Converter::chars_to_int(temp);
    temp += 4;

    password = Converter::chars_to_string(password_length, temp);
    temp += password_length;

    uint32_t image_name_length = Converter::chars_to_int(temp);
    temp += 4;

    image_name = Converter::chars_to_string(image_name_length, temp);
    temp += image_name_length;

    uint32_t array_count = Converter::chars_to_int(temp);
    temp += 4;

    for(unsigned i = 0; i < array_count; i++) {
        ImageView new_view;

        uint32_t username_length = Converter::chars_to_int(temp);
        temp += 4;

        new_view.username = Converter::chars_to_string(username_length, temp);
        temp += username_length;

        new_view.canView = Converter::chars_to_bool(temp);
        temp += 1;

        arr.push_back(new_view);
    }
}

// Retreive available services API
char* API::retreiveImages_arguemntsToArray(const std::string& password, uint32_t& args_size) {
    uint32_t password_length = uint32_t(password.length());

    args_size = 4 + password_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for get users API failed");

    char * pointer = args;

    Converter::int_to_chars((password_length), pointer);
    pointer += 4;

    Converter::string_to_chars(password, pointer);
    pointer += password_length;

    return args;
}
void API::retreiveImages_arrayToArguments(char* args, std::string& password) {
    char * pointer = args;

    uint32_t password_length = Converter::chars_to_int(pointer);
    pointer += 4;

    password = Converter::chars_to_string(password_length, pointer);
    pointer += password_length;
}
char* API::retreiveImages_resultsToArray(const std::vector<ImageReference>& available_images, uint32_t& args_size) {
    args_size = 0;

    uint32_t vector_size = (uint32_t)(available_images.size());
    args_size += 4;

    for(unsigned j = 0; j < vector_size; j++) {
        args_size += 4 + available_images[j].image_name.length() + 4 + available_images[j].owner_username.length();
    }

    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for get available images API failed");
    char * pointer = args;

    Converter::int_to_chars(vector_size, pointer);
    pointer += 4;

    for(unsigned j = 0; j < available_images.size(); j++) {
        uint32_t username_length = (uint32_t)(available_images[j].owner_username.length());
        uint32_t image_length = (uint32_t)(available_images[j].image_name.length());

        Converter::int_to_chars(username_length, pointer);
        pointer += 4;

        Converter::string_to_chars(available_images[j].owner_username, pointer);
        pointer += username_length;

        Converter::int_to_chars(image_length, pointer);
        pointer += 4;

        Converter::string_to_chars(available_images[j].image_name, pointer);
        pointer += image_length;
    }

    return args;
}
void API::retreiveImages_arrayToResults(char* args, std::vector<ImageReference>& available_images) {
    char * pointer = args;

    available_images.resize(0);

    uint32_t vector_size = Converter::chars_to_int(pointer);
    pointer += 4;

    for(unsigned j = 0; j < vector_size; j++) {
        ImageReference new_img;

        uint32_t username_length = Converter::chars_to_int(pointer);
        pointer += 4;

        new_img.owner_username = Converter::chars_to_string(username_length, pointer);
        pointer += username_length;

        uint32_t image_length = Converter::chars_to_int(pointer);
        pointer += 4;

        new_img.image_name = Converter::chars_to_string(image_length, pointer);
        pointer += image_length;

        available_images.push_back(new_img);
    }
}

// Retreive statistics API
char* API::retrievedStatistics_resultsToArray(const std::vector<Statistic>& user_statistics, uint32_t& args_size) {
    // Each entry in the user_statistics represents  views_No, imag_name, username
    // so the convention is views_No, then image_name_length then image_name then username_length then username
    args_size = 0;
    args_size += 4;

    uint32_t arr_length = (uint32_t)(user_statistics.size());
    for(int j = 0; j < arr_length; j++) {
        args_size += 4 + (uint32_t)(user_statistics[j].image_name.length()) + 4;
    }

    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for requestImage API failed");
    char* temp = args;

    Converter::int_to_chars(arr_length, temp);
    temp += 4;

    for(unsigned i = 0 ; i < user_statistics.size(); i++) {
        uint32_t image_name_length = (uint32_t)(user_statistics[i].image_name.length());

        Converter::int_to_chars(image_name_length, temp);
        temp += 4;

        Converter::string_to_chars(user_statistics[i].image_name, temp);
        temp += image_name_length;

        Converter::int_to_chars(user_statistics[i].views_no, temp);
        temp += 4;
    }

    return args;
}
void API::retrievedStatistics_arrayToResults(char* args, std::vector<Statistic>& user_statistics) {
    user_statistics.resize(0);

    char* temp = args;

    uint32_t arr_length = Converter::chars_to_int(temp);
    temp += 4;

    for(unsigned int i = 0 ; i < arr_length; i++) {
        Statistic new_statistic;

        uint32_t image_name_length = Converter::chars_to_int(temp);
        temp += 4;

        new_statistic.image_name = Converter::chars_to_string(image_name_length, temp);
        temp += image_name_length;

        new_statistic.views_no = Converter::chars_to_int(temp);
        temp += 4;

        user_statistics.push_back(new_statistic);
    }
}

// Request views API
char* API::requestviews_argumentsToArray(const ImageReference& image, uint32_t views_no , uint32_t& args_size) {
    std::string image_name = image.image_name;
    std::string username = image.owner_username;

    uint32_t image_name_length = (uint32_t)(image_name.length());
    uint32_t username_length = (uint32_t)(username.length());

    args_size = 4 + username_length + 4 + image_name_length + 4;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for request additional views API failed");
    char * pointer = args;

    Converter::int_to_chars(username_length, pointer);
    pointer += 4;

    Converter::string_to_chars(username, pointer);
    pointer += username_length;

    Converter::int_to_chars(image_name_length, pointer);
    pointer += 4;

    Converter::string_to_chars(image_name, pointer);
    pointer += image_name_length;

    Converter::int_to_chars(views_no, pointer);
    pointer += 4;

    return args;
}
void API::requestviews_arrayToArguments(char* args, ImageReference& image, uint32_t& views_no) {
    char * pointer = args;

    uint32_t username_length = Converter::chars_to_int(pointer);
    pointer += 4;

    image.owner_username = Converter::chars_to_string(username_length, pointer);
    pointer += username_length;

    uint32_t image_name_length = Converter::chars_to_int(pointer);
    pointer += 4;

    image.image_name = Converter::chars_to_string(image_name_length, pointer);
    pointer += image_name_length;

    views_no = Converter::chars_to_int(pointer);
    pointer += 4;
}
char* API::grantviews_argumentsToArray(const ImageReference& image, const uint32_t views_no, uint32_t& args_size) {
    std::string image_name = image.image_name;
    std::string username = image.owner_username;

    uint32_t image_name_length = (uint32_t)(image_name.length());
    uint32_t username_length = (uint32_t)(username.length());

    args_size = 4 + username_length + 4 + image_name_length + 4;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for grant additional views API failed");
    char * pointer = args;

    Converter::int_to_chars(username_length, pointer);
    pointer += 4;

    Converter::string_to_chars(username, pointer);
    pointer += username_length;

    Converter::int_to_chars(image_name_length, pointer);
    pointer += 4;

    Converter::string_to_chars(image_name, pointer);
    pointer += image_name_length;

    Converter::int_to_chars(views_no, pointer);
    pointer += 4;

    return args;
}
void API::grantviews_arrayToArguments(char* args, ImageReference& image, uint32_t& views_no) {
    char * pointer = args;

    uint32_t username_length = Converter::chars_to_int(pointer);
    pointer += 4;

    image.owner_username = Converter::chars_to_string(username_length, pointer);
    pointer += username_length;

    uint32_t image_name_length = Converter::chars_to_int(pointer);
    pointer += 4;

    image.image_name = Converter::chars_to_string(image_name_length, pointer);
    pointer += image_name_length;

    views_no = Converter::chars_to_int(pointer);
    pointer += 4;
}

// Update views API
char* API::updateViews_argumentsToArray(const PendingQuotaChange& change, uint32_t& args_size) {
    uint32_t owner_username_length = uint32_t(change.remote_img_ref.img_ref.owner_username.length());
    uint32_t image_length = uint32_t(change.remote_img_ref.img_ref.image_name.length());
    uint32_t shared_with_username_length = uint32_t(change.remote_img_ref.shared_with_username.length());

    args_size = 4 + owner_username_length + 4 + image_length + 4 + shared_with_username_length + 4 ;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for submit pending quota change API failed");

    char * pointer = args;

    Converter::int_to_chars(owner_username_length, pointer);
    pointer += 4;

    Converter::string_to_chars(change.remote_img_ref.img_ref.owner_username, pointer);
    pointer += owner_username_length;

    Converter::int_to_chars(image_length, pointer);
    pointer += 4;

    Converter::string_to_chars(change.remote_img_ref.img_ref.image_name, pointer);
    pointer += image_length;

    Converter::int_to_chars(shared_with_username_length, pointer);
    pointer += 4;

    Converter::string_to_chars(change.remote_img_ref.shared_with_username, pointer);
    pointer += shared_with_username_length;

    Converter::int_to_chars(change.new_quota, pointer);
    pointer += 4;

    return args;
}
void API::updateViews_arrayToArguments(char* args, PendingQuotaChange& change) {
    uint32_t sender_username_length, image_length, shared_with_username_length;
    char * pointer = args;

    sender_username_length = Converter::chars_to_int(pointer);
    pointer += 4;

    change.remote_img_ref.img_ref.owner_username = Converter::chars_to_string(sender_username_length, pointer);
    pointer += sender_username_length;

    image_length = Converter::chars_to_int(pointer);
    pointer += 4;

    change.remote_img_ref.img_ref.image_name = Converter::chars_to_string(image_length, pointer);
    pointer += image_length;

    shared_with_username_length = Converter::chars_to_int(pointer);
    pointer += 4;

    change.remote_img_ref.shared_with_username = Converter::chars_to_string(shared_with_username_length, pointer);
    pointer += shared_with_username_length;

    change.new_quota = Converter::chars_to_int(pointer);
}


// Request image API
char* API::requestImage_argumentsToArray(const ImageReference& image, const uint32_t views_no , uint32_t& args_size) {
    std::string image_name = image.image_name;
    std::string username = image.owner_username;

    uint32_t image_name_length = (uint32_t)(image_name.length());
    uint32_t username_length = (uint32_t)(username.length());

    args_size = 4 + username_length + 4 + image_name_length + 4;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for requestImage API failed");
    char * pointer = args;

    Converter::int_to_chars(username_length, pointer);
    pointer += 4;

    Converter::string_to_chars(username, pointer);
    pointer += username_length;

    Converter::int_to_chars(image_name_length, pointer);
    pointer += 4;

    Converter::string_to_chars(image_name, pointer);
    pointer += image_name_length;

    Converter::int_to_chars(views_no, pointer);
    pointer += 4;

    return args;
}
void API::requestImage_arrayToArguments(char* args, ImageReference& image, uint32_t& views_no) {
    char * pointer = args;

    uint32_t username_length = Converter::chars_to_int(pointer);
    pointer += 4;

    std::cout << "The username length is " << username_length << std::endl;

    image.owner_username = Converter::chars_to_string(username_length, pointer);
    pointer += username_length;

    std::cout << "The username is " << image.owner_username << std::endl;

    uint32_t image_name_length = Converter::chars_to_int(pointer);
    pointer += 4;

    std::cout << "The image name length is " << image_name_length << std::endl;

    image.image_name = Converter::chars_to_string(image_name_length, pointer);
    pointer += image_name_length;

    std::cout << "The image is " << image.image_name << std::endl;

    views_no = Converter::chars_to_int(pointer);
    pointer += 4;

    std::cout << "The views are " << views_no << std::endl;

    std::cout << "Unmarshalled message succesfully" << std::endl;
}

// Reply with image API
char* API::replyWithImage_argumentsToArray(const std::string& image, RemoteImageReference RIR, uint32_t& args_size) {
    uint32_t image_length = uint32_t(image.length());
    uint32_t shared_With_username_length = uint32_t(RIR.shared_with_username.length());
    uint32_t owner_username_length = uint32_t(RIR.img_ref.owner_username.length());
    uint32_t imagename_length = uint32_t(RIR.img_ref.image_name.length());


    args_size = 1 + 4 + image_length+ 4 + shared_With_username_length+ 4 + owner_username_length+4 + imagename_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for RplyWithImage API failed");

    char * pointer = args;

    Converter::int_to_chars(shared_With_username_length, pointer);
    pointer+=4;
    Converter::string_to_chars(RIR.shared_with_username, pointer);
    pointer+=shared_With_username_length;

    Converter::int_to_chars(owner_username_length, pointer);
    pointer+=4;
    Converter::string_to_chars(RIR.img_ref.owner_username, pointer);
    pointer+=owner_username_length;

    Converter::int_to_chars(imagename_length, pointer);
    pointer+=4;
    Converter::string_to_chars(RIR.img_ref.image_name, pointer);
    pointer+=imagename_length;

    Converter::bool_to_chars(true, pointer);
    pointer += 1;

    Converter::int_to_chars((image_length), pointer);
    pointer += 4;
    Converter::string_to_chars(image, pointer);
    pointer+=image_length;

    return args;
}

char* API::rejectShareRequest_argumentsToArray(RemoteImageReference RIR, uint32_t& args_size) {
    uint32_t shared_With_username_length = uint32_t(RIR.shared_with_username.length());
    uint32_t owner_username_length = uint32_t(RIR.img_ref.owner_username.length());
    uint32_t imagename_length = uint32_t(RIR.img_ref.image_name.length());

    args_size = 1 + 4 + shared_With_username_length + 4 + owner_username_length + 4 + imagename_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for RplyWithImage API failed");

    char * pointer = args;

    Converter::int_to_chars(shared_With_username_length, pointer);
    pointer+=4;
    Converter::string_to_chars(RIR.shared_with_username, pointer);
    pointer+=shared_With_username_length;

    Converter::int_to_chars(owner_username_length, pointer);
    pointer+=4;
    Converter::string_to_chars(RIR.img_ref.owner_username, pointer);
    pointer+=owner_username_length;

    Converter::int_to_chars(imagename_length, pointer);
    pointer+=4;
    Converter::string_to_chars(RIR.img_ref.image_name, pointer);
    pointer+=imagename_length;

    Converter::bool_to_chars(false, pointer);
    pointer += 1;

    return args;
}

void API::replyWithImage_arrayToArguments(char* args, std::string& image,  RemoteImageReference& RIR, bool& result) {
    char * pointer = args;

    uint32_t shared_With_username_length = Converter::chars_to_int(pointer);
    pointer += 4;
    RIR.shared_with_username = Converter::chars_to_string(shared_With_username_length, pointer);
    pointer += shared_With_username_length;

    uint32_t owner_username_length = Converter::chars_to_int(pointer);
    pointer += 4;
    RIR.img_ref.owner_username = Converter::chars_to_string(owner_username_length, pointer);
    pointer += owner_username_length;

    uint32_t imagename_length = Converter::chars_to_int(pointer);
    pointer += 4;
    RIR.img_ref.image_name = Converter::chars_to_string(imagename_length, pointer);
    pointer += imagename_length;

    result = Converter::chars_to_bool(pointer);
    pointer += 1;

    if(result) {
        uint32_t image_length = Converter::chars_to_int(pointer);
        pointer += 4;
        image = Converter::chars_to_string(image_length, pointer);
        pointer += image_length;
    } else {
        image = "";
    }
}

// Acknowledge receipt of image API
char* API::replyWithRIRAck_argumentsToArray(const RemoteImageReference& RIR, uint32_t views_no, uint32_t& args_size) {
    uint32_t shared_With_username_length = uint32_t(RIR.shared_with_username.length());
    uint32_t owner_username_length = uint32_t(RIR.img_ref.owner_username.length());
    uint32_t imagename_length = uint32_t(RIR.img_ref.image_name.length());

    args_size = 4 + shared_With_username_length + 4 + owner_username_length + 4 + imagename_length + 4;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for ReplyWithRIRAck API failed");

    char * pointer = args;

    Converter::int_to_chars(shared_With_username_length, pointer);
    pointer += 4;
    Converter::string_to_chars(RIR.shared_with_username, pointer);
    pointer += shared_With_username_length;

    Converter::int_to_chars(owner_username_length, pointer);
    pointer += 4;
    Converter::string_to_chars(RIR.img_ref.owner_username, pointer);
    pointer += owner_username_length;

    Converter::int_to_chars(imagename_length, pointer);
    pointer += 4;
    Converter::string_to_chars(RIR.img_ref.image_name, pointer);
    pointer += imagename_length;

    Converter::int_to_chars(views_no, pointer);
    pointer += 4;

    return args;
}

void API::replyWithRIRAck_arrayToArguments(char * args, RemoteImageReference& RIR, uint32_t& views_no) {
    uint32_t shared_With_username_length;
    uint32_t owner_username_length;
    uint32_t imagename_length;

    char * pointer = args;

    shared_With_username_length = Converter::chars_to_int(pointer);
    pointer += 4;
    RIR.shared_with_username = Converter::chars_to_string(shared_With_username_length, pointer);
    pointer += shared_With_username_length;

    owner_username_length = Converter::chars_to_int(pointer);
    pointer += 4;
    RIR.img_ref.owner_username = Converter::chars_to_string(owner_username_length, pointer);
    pointer += owner_username_length;

    imagename_length = Converter::chars_to_int(pointer);
    pointer += 4;
    RIR.img_ref.image_name = Converter::chars_to_string(imagename_length, pointer);
    pointer += imagename_length;

    views_no = Converter::chars_to_int(pointer);
    pointer += 4;
}


// ayaaaaaa!!!!!!!!!!!!!!!!!!!!!!!!!!11


char* API::retrievedStatistics_argumentsToArray(const std::vector<Statistic>& user_statistics, uint32_t& args_size)
{
    // Each entry in the user_statistics represents  views_No, imag_name, username
    // so the convention is views_No, then image_name_length then image_name then username_length then username
    args_size = 0;

    args_size+= 4;
    for(unsigned int j = 0; j < user_statistics.size(); j++)
    {
        args_size += 4+ 4+ user_statistics[j].image_name.length()+ 4+ user_statistics[j].shared_with_username.length();
    }


    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating arguments for requestImage API failed");

    char* temp = args;

    Converter::int_to_chars(user_statistics.size(), temp);
    temp += 4;

    for(unsigned int i = 0 ; i < user_statistics.size(); i++)
    {
        Converter::int_to_chars((user_statistics[i].views_no), temp);
        uint32_t image_name_length = user_statistics[i].image_name.length();
        Converter::int_to_chars((image_name_length), temp+4);
        Converter::string_to_chars(user_statistics[i].image_name, temp+8);

        uint32_t username_length = user_statistics[i].shared_with_username.length();
        Converter::int_to_chars((username_length), temp+ 8+ image_name_length);
        Converter::string_to_chars(user_statistics[i].shared_with_username, temp+ 12 + image_name_length);

        temp = temp + 12 + image_name_length + username_length;
    }


    return args;
}

void  API::retrievedStatistics_arrayToArguments(char* args, uint32_t args_size ,std::vector<Statistic>& user_statistics)
{
    user_statistics.resize(0);
    std::cout<< "In API retrievedStatistics_arrayToArguments: " << std::endl;

    char* temp = args;

    uint32_t vector_size;
    vector_size = Converter::chars_to_int(temp);
    temp += 4;

    user_statistics.resize(vector_size);
    for(unsigned int i = 0; i < vector_size; i++)
    {

        std::cout<< "In API loop args is: "<< args << std::endl;
        user_statistics[i].views_no = (Converter::chars_to_int(temp));
        std::cout<< "after inserting views_no: " << user_statistics[i].views_no << std::endl;
        uint32_t image_name_length = (Converter::chars_to_int(temp+4));
        std::cout<< "after getting image length: " << image_name_length<< std::endl;
        user_statistics[i].image_name = Converter::chars_to_string(image_name_length, temp + 8);
        std::cout<< "after inserting image name: " << user_statistics[i].image_name << std::endl;


        uint32_t username_length = (Converter::chars_to_int(temp + 8 + image_name_length));
         std::cout<< "after getting username length: " << username_length<< std::endl;
        user_statistics[i].shared_with_username = Converter::chars_to_string(username_length, temp + 12 + image_name_length);
         std::cout<< "after inserting username: " <<user_statistics[i].shared_with_username <<std::endl;
        temp = temp + 12 + image_name_length + username_length;
         std::cout<< "after getting temp: " << temp << std::endl;
    }

}

char* API::announceImage_argumentsToArray(const std::string& password, const std::string& image_name, uint32_t& args_size) {
    uint32_t password_length = (uint32_t) (password.length());
    uint32_t image_name_length = (uint32_t) (image_name.length());

    args_size = 4 + password_length + 4 + image_name_length;
    char* args = new char[args_size];
    ErrorHandler::checkPointer(args, "Memory allocation while creating results for annnounce image API failed");
    char* temp = args;

    Converter::int_to_chars(password_length, temp);
    temp += 4;

    Converter::string_to_chars(password, temp);
    temp += password_length;

    Converter::int_to_chars(image_name_length, temp);
    temp += 4;

    Converter::string_to_chars(image_name, temp);
    temp += image_name_length;

    return args;

}
void API::announceImage_arrayToArguments(char * args, std::string& password, std::string& image_name) {
    char * temp = args;

    uint32_t password_length = Converter::chars_to_int(temp);
    temp += 4;

    password = Converter::chars_to_string(password_length, temp);
    temp += password_length;

    uint32_t image_name_length = Converter::chars_to_int(temp);
    temp += 4;

    image_name = Converter::chars_to_string(image_name_length, temp);
    temp += image_name_length;
}






