#ifndef SERVER_H
#define SERVER_H

#include "InterfaceDefintion.h"

#include "UDPServerSocket.hpp"
#include "Message.hpp"
#include <string>
//#include "ss.h"

class Server {
protected:
    UDPServerSocket * udpServerSocket;

  //  Message * doOperation(Message * received_message);

public:
    Server();
    Server(const Server& other);
    Server(Server&& other);

    Server(uint32_t _listen_ipAddress, uint16_t _listen_port);

    Message * receiveRequest();
    void sendReply(Message * reply);

    //int serveRequest();
    ~Server();
};

#endif // SERVER_H
