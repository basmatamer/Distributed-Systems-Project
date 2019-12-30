#ifndef Client_hpp
#define Client_hpp

#include <stdio.h>
#include "UDPClientSocket.hpp"
#include "Message.hpp"
#include "InterfaceDefintion.h"
//#include "ss.h"

#include "Configs.h"
#include <mutex>
#define MESSAGES_SENT_FILENAME (files_cache_path + "messages_sent.txt")

class Client
{
protected:
    UDPClientSocket * udpClientSocket;




    static std::mutex message_mutex;

    InternetAddress getReceiverAddress(const std::string& reciever_username);
    uint32_t assignMessageID();
    void rollBackMessageID();

    Message * _send(Message * _message, unsigned int timeout_usec);

public:
    std::string my_username;
    InternetAddress my_server_address;
    InternetAddress directory_service_address;
    std::string my_password;


    Client();
    Client(const Client& other);
    Client(Client&& other);
    Client(const std::string& my_username, const std::string& my_password, InternetAddress my_address, InternetAddress my_server_address, InternetAddress directory_service_address);
    Client(const std::string& my_username, InternetAddress my_address, InternetAddress my_server_address, InternetAddress directory_service_address);
    ~Client();

    Message * sendMessage(Message * _message, unsigned int timeout_usec);

    void setMyPassword(const std::string& _password);
    std::string getMyPassword() const;
};

#endif /* Client_hpp */
