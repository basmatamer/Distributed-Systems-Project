#ifndef APPLICATIONSERVER_H
#define APPLICATIONSERVER_H

#include "Server.hpp"

class ApplicationServer
{
private:
    static void handle_anyRequest(Message* request);

    static void handle_logout(Message* request);

    static void handle_textMessage(Message* request);

    static void handle_requestImage(Message* request);

    static void handle_changeQuota(Message* request);

    static void handle_sendImage(Message* request);
    static void save_image(bool result, std::string owner_username, std::string image_name, std::string* image_string);

    static void handle_requestViews(Message* request);
    static void update_image(std::string owner_username, std::string image_name, uint32_t new_quota);

    static void roam_service_server(Message* request);

public:
    static void serverMain(uint32_t ip_address, uint16_t port_number);
};

#endif // APPLICATIONSERVER_H
