#ifndef API_H
#define API_H

#include <string>
#include <vector>
#include "InterfaceDefintion.h"

class API {
public:
    // Text API
    static char* text_arguemntsToArray(const std::string& text_msg, uint32_t& args_size);
    static void text_arrayToArguments(char* args , std::string& text_msg);
    static char* text_resultsToArray(const std::string& reply_msg, uint32_t& args_size);
    static void text_arrayToResults(char* args , std::string& reply_msg);

    // Signup API
    static char* signup_argumentsToArray(const std::string& password, uint32_t& args_size);
    static void signup_arrayToArguments(char* args, std::string& password);
    static char* signup_resultsToArray(bool isAccepted, const std::string& text, uint32_t& args_size);
    static void signup_arrayToResults(char* args, bool& isAccepted, std::string& text);

    // Login API
    static char* login_argumentsToArray(const std::string& password, uint32_t& args_size);
    static void login_arrayToArguments(char* args, std::string& password);
    static char* login_resultsToArray(bool isAccepted, uint32_t& args_size);
    static void login_arrayToResults(char* args, bool& isAccepted);

    // Hi Server API
    static char* hiServer_argumentsToArray(const std::string& password, uint32_t ipAddress, uint16_t portNumber, uint32_t& args_size);
    static void hiServer_arrayToArguments(char* args, std::uint32_t& ipAddress, std::uint16_t& portNumber, std::string& password);
    static char* hiServer_resultsToArray(const std::vector<PendingQuotaChange>& changes, uint32_t& args_size);
    static void hiServer_arrayToResults(char* args, std::vector<PendingQuotaChange>& changes);

    // Remove pending quota changes API
    static char* removePendingQuotaChanges_argumentsToArray(const std::string& password, uint32_t& args_size);
    static void removePendingQuotaChanges_arrayToArguments(char* args, std::string& password);

    // Change Password API
    static char* changePassword_argumentsToArray(const std::string& old_password, const std::string& new_password, uint32_t& args_size);
    static void changePassword_arrayToArguments(char* args, std::string& old_password, std::string& new_password);
    static char* changePassword_resultsToArray(bool result, uint32_t& args_size);
    static void changePassword_arrayToResults(char* args, bool& result);

    // Submit pending quota change API
    static char* submitPendingQuotaChange_argumentsToArray(const std::string& password, PendingQuotaChange change, uint32_t& args_size);
    static void submitPendingQuotaChange_arrayToArguments(char* args, std::string& password, PendingQuotaChange& change);

    // Get Address of User API
    static char* mapUsernameToAddress_argumentsToArray(const std::string& password, const std::string& required_username, uint32_t& args_size);
    static void mapUsernameToAddress_arrayToArguments(char * args, std::string& password, std::string& required_username);
    static char* mapUsernameToAddress_resultsToArray(bool result, InternetAddress address, uint32_t& args_size);
    static void mapUsernameToaddress_arrayToResults(char* args, bool& result, InternetAddress& address);

    // Get Users API
    static char* getUsers_argumentsToArray(const std::string& password, uint32_t& args_size);
    static void getUsers_arrayToArguments(char* args, std::string& password);
    static char* getUsers_resultsToArray(const std::vector<std::string>& usernames, uint32_t& args_size);
    static void getUsers_arrayToResults(char* args, std::vector<std::string>& usernames);

    // Offer service to users API
    static char* allowUsertoView_argumentsToArray(const std::string& password, const std::vector<ImageView>& arr, const std::string& image_name, uint32_t& args_size);
    static void allowUsertoView_arrayToArguments(char* args, std::string& password, std::string& image_name, std::vector<ImageView>& arr);

    // Retreive available services
    static char* retreiveImages_arguemntsToArray(const std::string& password, uint32_t& args_size);
    static void retreiveImages_arrayToArguments(char* args, std::string& password);
    static char* retreiveImages_resultsToArray(const std::vector<ImageReference>& available_images, uint32_t& args_size);
    static void retreiveImages_arrayToResults(char* args, std::vector<ImageReference>& available_images);

    // Retreive statistics API
    // Request will not include any arguments
    static char* retrievedStatistics_resultsToArray(const std::vector<Statistic>& user_statistics, uint32_t& args_size);
    static void retrievedStatistics_arrayToResults(char* args, std::vector<Statistic>& user_statistics);

    // Request Additional Views API
    static char* requestviews_argumentsToArray(const ImageReference& image, uint32_t views_no , uint32_t& args_size);
    static void requestviews_arrayToArguments(char* args, ImageReference& image, uint32_t& views_no);
    static char* grantviews_argumentsToArray(const ImageReference& image, const uint32_t views_no, uint32_t& args_size);
    static void grantviews_arrayToArguments(char* args, ImageReference& image, uint32_t& views_no);

    // Update Views API
    static char* updateViews_argumentsToArray(const PendingQuotaChange& change, uint32_t& args_size);
    static void updateViews_arrayToArguments(char* args, PendingQuotaChange& change);

    // Request image API
    static char* requestImage_argumentsToArray(const ImageReference& image, uint32_t views_no, uint32_t& args_size);
    static void requestImage_arrayToArguments(char* args, ImageReference& image, uint32_t& views_no);

    // Reply with image API
    static char* replyWithImage_argumentsToArray(const std::string& image, RemoteImageReference RIR, uint32_t& args_size);
    static char* rejectShareRequest_argumentsToArray(RemoteImageReference RIR, uint32_t& args_size);

    static void replyWithImage_arrayToArguments(char* args, std::string& image,  RemoteImageReference& RIR, bool& result);

    // Acknowledge receipt of image API
    static char* replyWithRIRAck_argumentsToArray(const RemoteImageReference& RIR, uint32_t views_no, uint32_t& args_size);
    static void replyWithRIRAck_arrayToArguments(char * args, RemoteImageReference& RIR, uint32_t& views_no);

    // Acknowledge receipt of image API
    static char* announceImage_argumentsToArray(const std::string& password, const std::string& image_name, uint32_t& args_size);
    static void announceImage_arrayToArguments(char * args, std::string& password, std::string& image_name);




    // ayaaaa!!!!!!!!!!!!!!!!!!!!!!!!!!1
    static char* retrievedStatistics_argumentsToArray(const std::vector<Statistic>& user_statistics, uint32_t& args_size);
    static void  retrievedStatistics_arrayToArguments(char* args, uint32_t args_size ,std::vector<Statistic>& user_statistics);


};

#endif
