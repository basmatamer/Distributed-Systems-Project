//
//  UDPSocket.cpp
//  Distributed ex1
//
//  Created by MacBook Pro on 9/25/18.
//  Copyright © 2018 MacBook Pro. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cerrno>
#include <vector>

#include "UDPSocket.hpp"
#include "ErrorHandler.hpp"
#include "LocalDirectory.hpp"

#include "ourexception.h"

UDPSocket::UDPSocket():enabled(true) {
    /// Check http://www.tutorialspoint.com/unix_system_calls/socket.htm
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        throw SocketCreationFailedException("Socket creation failed");
    }


    // zero out the structure
    memset((char *) &myAddr, 0, sizeof(myAddr));
    memset((char *) &peerAddr, 0, sizeof(peerAddr));

}

UDPSocket::UDPSocket(const UDPSocket& other):sock(other.sock), length_of_received(other.length_of_received), myAddr(other.myAddr), peerAddr(other.peerAddr), enabled(other.enabled) {
    ErrorHandler::displayWarning("Attempting to copy a UDP Socket");
}
UDPSocket::UDPSocket(UDPSocket&& other):sock(other.sock), length_of_received(other.length_of_received), myAddr(other.myAddr), peerAddr(other.peerAddr), enabled(other.enabled) {
    ErrorHandler::displayWarning("Attempting to move a UDP Socket");
}

UDPSocket::UDPSocket(uint32_t _myAddr, uint16_t _myPort):enabled(true) {
    /// Check http://www.tutorialspoint.com/unix_system_calls/socket.htm
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        throw SocketCreationFailedException("Socket creation failed");
    }

    // zero out the structure
    memset((char *) &myAddr, 0, sizeof(myAddr));
    memset((char *) &peerAddr, 0, sizeof(peerAddr));

    if(!_initializeToReceive(_myAddr, _myPort)) {
        throw SocketCreationFailedException("Socket creation failed");
    }
}

bool UDPSocket::_initializeToReceive(uint32_t _myAddress, uint16_t _myPort) {
    memset((char *) &myAddr, 0, sizeof(myAddr));

    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(_myPort);
    myAddr.sin_addr.s_addr = htonl(_myAddress);

    // Check https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
    // Check https://linux.die.net/man/3/inet_addr

    //std::cout << "Attempting to bind at " << LocalDirectory::IP_toString(_myAddress) << " (" << _myPort << ")" << std::endl;

    if(bind(sock, (sockaddr*) &myAddr, sizeof(myAddr)) == -1) {
        throw SocketBindingFailedException("Binding my socket failed with error " + std::to_string(errno));
        return false;
    }

    return true;
}

bool UDPSocket::_initializeToSend(uint32_t _peerAddress, uint16_t _peerPort) {
    memset((char *) &peerAddr, 0, sizeof(peerAddr));

    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(_peerPort);
    peerAddr.sin_addr.s_addr = htonl(_peerAddress);

    return true;
}


uint32_t UDPSocket::getMyAddress() const{
    return ntohl(myAddr.sin_addr.s_addr);
}
uint16_t UDPSocket::getMyPort () const{
    return(ntohs(myAddr.sin_port));
}
uint32_t UDPSocket::getPeerAddress() const{
    return ntohl(peerAddr.sin_addr.s_addr);
}
uint16_t UDPSocket::getPeerPort () const{
    return(ntohs(peerAddr.sin_port));

}

void UDPSocket::enable() {
    enabled = true;
}
void UDPSocket::disable() {
    enabled = false;
}
bool UDPSocket::isEnabled() const{
    return enabled;
}

int UDPSocket::getSocketHandler() const{
    return sock;
}
UDPSocket::~UDPSocket() {
    close(sock);
}

void UDPSocket::_setTimeOutValue(unsigned int usec) {
    // Check https://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
    struct timeval tv;
    tv.tv_sec = usec / 1000;
    tv.tv_usec = usec % 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
}

int UDPSocket::_receiveWithTimeout(char * buffer, size_t buffer_length,  uint32_t& source_ipAddress, uint16_t& source_portNumber, unsigned int usec) {
    if(usec==0) {
        int result = _receive(buffer, buffer_length, source_ipAddress, source_portNumber);
        return result;
     }

     fd_set rfds;
     struct timeval tv;
     int retval;

     /* Watch stdin (fd 0) to see when it has input. */
     FD_ZERO(&rfds);
     FD_SET(sock, &rfds);

     /* Wait up to five seconds. */
     tv.tv_sec = usec/1000;
     tv.tv_usec = 0;

     retval = select(sock + 1, &rfds, NULL, NULL, &tv);
    /* Don’t rely on the value of tv now! */


     if (retval == -1) {
         //perror("select()");
         std::cout << "Select system call caused an error" << std::endl;
         return -1;
     }
     else if (retval > 0) {
         printf("Data is available now.\n");

         // assign to buffer

         return _receive(buffer, buffer_length, source_ipAddress, source_portNumber);

         /* FD_ISSET(0, &rfds) will be true. */
     }
     else {
         return -1;
         printf("No data within timeout seconds.\n");
     }

     return -1;
}

int UDPSocket::_receiveWithTimeout2(char * buffer, size_t buffer_length,  uint32_t& source_ipAddress, uint16_t& source_portNumber, unsigned int usec) {
    _setTimeOutValue(usec);

//    std::cout << "Time out is " << usec << "ms" << std::endl;

    int len = _receive(buffer, buffer_length, source_ipAddress, source_portNumber);


    if (len == -1) {
        ErrorHandler::displayWarning("Timeout: No message was received");
    }
    _setTimeOutValue(0);
    return len;
}


int UDPSocket::_receive(char * buffer, size_t buffer_length, uint32_t& source_ipAddress, uint16_t& source_portNumber) {
    // documentation of recvfrom http://pubs.opengroup.org/onlinepubs/009604599/functions/recvfrom.html
    socklen_t addr_len = sizeof(peerAddr);
    ssize_t len = recvfrom(this->sock, buffer, buffer_length, 0, (sockaddr *) &peerAddr, &addr_len);
//    std::cout << "The buffer length is " << buffer_length << std::endl;
    std::cout << "I have received " << len << " bytes" << std::endl;

    source_ipAddress = ntohl(peerAddr.sin_addr.s_addr);
    source_portNumber = ntohs(peerAddr.sin_port);

    return int(len);
}

int UDPSocket::_send(const char * message, size_t message_length, uint32_t destination_ipAddress, uint16_t destination_portNumber) {
    _initializeToSend(destination_ipAddress, destination_portNumber);
    // documentation of sendto http://pubs.opengroup.org/onlinepubs/7908799/xns/sendto.html
    ssize_t len = sendto(this->sock, message, message_length, 0, (sockaddr *) &peerAddr, sizeof(peerAddr));
    if (len == -1) {
        // ErrorHandler::displayWarning("No message was sent");
       // std::cout << len << " bytes were sent" << std::endl;
    }
    return int(len);
}
