#ifndef InterfaceDefintion_h
#define InterfaceDefintion_h

#include <string>
#include <vector>
#include "Configs.h"

#define SERVER_PORT 2222
#define CLIENT_PORT 3333
#define DIRECTORY_PORT 4444

#define BUFFER_SIZE (62 * 1024)
#define PACKET_DATA_SIZE (60 * 1024)
#define DEFAULT_TIMEOUT_USEC (4000)
#define DEFAULT_TIMEOUT_ACK_USEC (1000)
#define DEFAULT_TIMEOUT_ACK_DIRECTORY_USEC (4000)
#define SEND_IMAGE_TIMEOUT_USEC (10000)

#define DEFAULT_IMAGE_SIZE 30789588

#define MAX_FILE_SIZE (DEFAULT_IMAGE_SIZE / 9)

#define DIRECTORY_SERVICE_USERNAME "directory"

#define RESEND_NO 5   // number of times to issue resend before raising an exception
#define STATISTICS_LOG_FILENAME (files_cache_path + "statistics_log.txt")

#define PENDING_REQUESTS_FILENAME (files_cache_path + "pending_requests.txt")

#define ACCEPTED_REQUESTS_FILENAME (files_cache_path + "accepted_requests.txt")

#define SIGNED_UP_FILENAME (app_path + ".signed_up.txt")

#define WINDOW_SIZE 5

#define ROBOT_SLEEP_SECONDS (5 * 60)

enum OperationID {
    // Testing Operations
    RESERVED = 0,
    EMPTY,
    TEXT_MESSAGE,

    // Directory Service Operations
    SIGNUP,
    LOGIN,
    HI_SERVER,
    REMOVE_PENDING_QUOTA_CHANGES_REQUEST,
    CHANGE_PASSWORD,
    SUBMIT_PENDING_QUOTA_CHANGE_REQUEST,
    GET_ADDRESS_OF_USER,
    GET_USERNAMES,
    OFFER_SERVICE,
    GET_AVAILABLE_SERVICES,

    // Special Directory Service Operations
    TURN_OFF_DIRECTORY_SERVICE,

    // P2P Operations
    REQUEST_IMAGE,
    REQUEST_ADDITIONAL_VIEWS,
    RETREIVE_STATISTICS,
    UPDATE_VIEWS_COUNTER,
    SEND_IMAGE,

    // Loopback Operations
    LOGOUT,

    // New operations
    ANNOUNCE_IMAGE
};

enum MessageTargetType {
    UNKOWN = 0,
    MYSELF,
    ANOTHER_PEER,
    DIRECTORY_SERVICE
};

MessageTargetType getTargetType(OperationID op_id);

struct InternetAddress {
    uint32_t ip;
    uint16_t port;

    bool operator==(const InternetAddress& other) const{
        return (ip == other.ip && port == other.port);
    }

    bool operator<(const InternetAddress& other) const{
        if (ip < other.ip) {
            return true;
        } else if (port < other.port) {
            return true;
        } else {
            return false;
        }
    }
};

struct ImageReference {
    std::string owner_username;
    std::string image_name;

    bool operator==(const ImageReference& other) const{
        return (owner_username == other.owner_username && image_name == other.image_name);
    }

};

struct RemoteImageReference {
    ImageReference img_ref;
    std::string shared_with_username;
};

struct ImageView {
    std::string username;
    bool canView;
};

// Struct for structure of statistics
struct Statistic {
    std::string shared_with_username;
    std::string image_name;
    int views_no;
};

struct PendingQuotaChange {
    RemoteImageReference remote_img_ref;
    // TODO check this implementation
    uint32_t new_quota;
};

//aya!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

struct ImageQuota
{
    ImageReference img;
    uint32_t agreed_quota;
};

struct Log_entry
{
    std::string shared_with_username;
    ImageQuota shared_with_images;
};

struct ImageUsersViews
{
    std::string shared_with_username;
    uint32_t actual_views;
};

#endif /* InterfaceDefintion_h */
