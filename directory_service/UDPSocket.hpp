#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <iostream>
#include <string>
#include <unistd.h>
#include <time.h>


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include "Message.hpp"

class UDPSocket {
protected:
    int sock;
    int length_of_received;    // length of the received message
    sockaddr_in myAddr;
    sockaddr_in peerAddr;
    bool enabled;
    pthread_mutex_t mutex;

    bool _initializeToReceive (uint32_t _myAddr, uint16_t _myPort);
    bool _initializeToSend (uint32_t _peerAddr, uint16_t _peerPort);

    void _setTimeOutValue(unsigned int usec);

    int _send(const char * message, size_t message_length, uint32_t destination_ipAddress, uint16_t destination_portNumber);

    int _receiveWithTimeout(char * buffer, size_t buffer_length, uint32_t& source_ipAddress, uint16_t& source_portNumber, unsigned int usec);
    int _receiveWithTimeout2(char * buffer, size_t buffer_length, uint32_t& source_ipAddress, uint16_t& source_portNumber, unsigned int usec);
    int _receive(char * buffer, size_t buffer_length, uint32_t& source_ipAddress, uint16_t& source_portNumber);

public:
    UDPSocket();
    UDPSocket(const UDPSocket& other);
    UDPSocket(UDPSocket&& other);
    UDPSocket(uint32_t _myAddr, uint16_t _myPort);

    ~UDPSocket();

    uint32_t getMyAddress() const;
    uint16_t getMyPort() const;
    uint32_t  getPeerAddress() const;
    uint16_t getPeerPort() const;
    void enable();
    void disable();
    bool isEnabled() const;
    void lock();
    void unlock();
    int getSocketHandler() const;

};
#endif // UDPSOCKET_H
