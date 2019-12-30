#include "Server.hpp"
#include "ErrorHandler.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include "InterfaceDefintion.h"
#include "API.hpp"

#include "ourexception.h"

Server::Server():udpServerSocket(NULL) {
    ErrorHandler::displayWarning("Attempting to create a default Server");
    udpServerSocket = NULL;
}

Server::Server(const Server& other) {
    ErrorHandler::displayWarning("Attempting to copy a Server");
    udpServerSocket = new UDPServerSocket(*(other.udpServerSocket));
    ErrorHandler::checkPointer(udpServerSocket, "Memory allocation while copying a Server failed");
}

Server::Server(Server&& other):udpServerSocket(other.udpServerSocket) {
    ErrorHandler::displayWarning("Attempting to move a Server");
    other.udpServerSocket = NULL;
}

Server::Server(uint32_t _listen_ipAddress, uint16_t _listen_port) {
    udpServerSocket = new UDPServerSocket(_listen_ipAddress, _listen_port);
    ErrorHandler::checkPointer(udpServerSocket, "Memory allocation while creating a Server failed");
}

Message * Server::receiveRequest() {
    ErrorHandler::checkPointer(udpServerSocket, "Attempting to serve a request while the UDP Server Socket is NULL. Fatal Error.");
    // std::cout << "Waiting to receive a message .." << std::endl;

    // Receive the message
    try {
        return udpServerSocket->receive(BUFFER_SIZE);
    } catch(const SendingFailedException& except) {
        std::cout << except.what() << std::endl;
        throw except;
    }
}


void Server::sendReply(Message * reply) {
    if(reply == NULL) {
        // std::cout << "No reply was formulated .." << std::endl;
    } else {
        ErrorHandler::checkPointer(udpServerSocket, "Attempting to serve a request while the UDP Server Socket is NULL. Fatal Error.");
        // Send the reply
        try {
            udpServerSocket->reply(reply);
        } catch(const SendingFailedException& except) {
            std::cout << "Server::sendReply " << except.what() << std::endl;
        }

    }
}

Server::~Server() {
    if(udpServerSocket != NULL) {
        delete udpServerSocket;
        udpServerSocket = NULL;
    }
}
