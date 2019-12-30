#include "LocalDirectory.hpp"
#include "ErrorHandler.hpp"

#include <arpa/inet.h>
#include <string>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <cstring>

std::string LocalDirectory::IP_toString(uint32_t ip) {
    in_addr ip_addr;
    ip_addr.s_addr = htonl(ip);

    return std::string(inet_ntoa(ip_addr));
}
uint32_t LocalDirectory::IP_fromString(const std::string& ip_string) {
    in_addr ip_addr;
    if (inet_aton(ip_string.c_str(), &ip_addr) == 0) {
        ErrorHandler::die(ip_string + " is an invalid ip address");
    }
    return (uint32_t)(ntohl(ip_addr.s_addr));
}

InternetAddress LocalDirectory::getEmptyAddress() {
    InternetAddress default_address;
    default_address.ip = 0;
    default_address.port = 0;
    return default_address;
}

LocalDirectory::LocalDirectory():my_address(getEmptyAddress()), directoryService_address(getEmptyAddress()) {
    cached_addresses.clear();
}
LocalDirectory::LocalDirectory(const LocalDirectory& other):my_address(other.my_address), directoryService_address(other.directoryService_address), cached_addresses(other.cached_addresses) {

}
LocalDirectory::LocalDirectory(LocalDirectory&& other):my_address(other.my_address), directoryService_address(other.directoryService_address), cached_addresses(other.cached_addresses) {
}

void LocalDirectory::setMyAddress(InternetAddress address) {
    my_address = address;
}
void LocalDirectory::setMyAddress(std::string ip_address, uint16_t port) {
    my_address.ip = IP_fromString(ip_address);
    my_address.port = port;
}
void LocalDirectory::setMyAddress(uint32_t ip_address, uint16_t port) {
    my_address.ip = ip_address;
    my_address.port = port;
}
InternetAddress LocalDirectory::getMyAddress() const {
    return my_address;
}
uint32_t LocalDirectory::getMyIP() const {
    return my_address.ip;
}
uint16_t LocalDirectory::getMyPort() const {
    return my_address.port;
}

void LocalDirectory::setDirectoryAddress(InternetAddress address) {
    directoryService_address = address;
}
void LocalDirectory::setDirectoryAddress(std::string ip_address, uint16_t port) {
    directoryService_address.ip = IP_fromString(ip_address);
    directoryService_address.port = port;
}
void LocalDirectory::setDirectoryAddress(uint32_t ip_address, uint16_t port) {
    directoryService_address.ip = ip_address;
    directoryService_address.port = port;
}
InternetAddress LocalDirectory::getDirectoryAddress() const {
    return directoryService_address;
}
uint32_t LocalDirectory::getDirectoryIP() const {
    return directoryService_address.ip;
}
uint16_t LocalDirectory::getDirectoryPort() const {
    return directoryService_address.port;
}

void LocalDirectory::setPeerAddress(const std::string& username, InternetAddress address) {
    cached_addresses[username] = address;
}
void LocalDirectory::setPeerAddress(const std::string& username, std::string ip_address, uint16_t port) {
    InternetAddress address;
    address.ip = IP_fromString(ip_address);
    address.port = port;
    cached_addresses[username] = address;
}
void LocalDirectory::setPeerAddress(const std::string& username, uint32_t ip_address, uint16_t port) {
    InternetAddress address;
    address.ip = ip_address;
    address.port = port;
    cached_addresses[username] = address;
}
InternetAddress LocalDirectory::getPeerAddress(const std::string& username) const {
    std::map<std::string,InternetAddress>::const_iterator it = cached_addresses.find(username);
    if(it == cached_addresses.end()) {
        return getEmptyAddress();
    } else {
        return cached_addresses.at(username);
    }
}
uint32_t LocalDirectory::getPeerIP(const std::string& username) const {
    return getPeerAddress(username).ip;
}
uint16_t LocalDirectory::getPeerPort(const std::string& username) const {
    return getPeerAddress(username).port;
}

std::vector<std::string> LocalDirectory::getAllPeerUsernames() const {
    std::vector<std::string> usernames;
    usernames.resize(0);
    for(std::map<std::string, InternetAddress>::const_iterator iter = cached_addresses.begin(); iter != cached_addresses.end(); ++iter) {
        usernames.push_back(iter->first);
    }
    return usernames;
}


uint32_t LocalDirectory::GetPrimaryIp() {
    socklen_t buflen = 16;
    char * buffer = new char[buflen];

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sock != -1);

    const char* kGoogleDnsIp = "8.8.8.8";
    uint16_t kDnsPort = 53;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    serv.sin_port = htons(kDnsPort);

    int err = connect(sock, (const sockaddr*) &serv, sizeof(serv));
    assert(err != -1);

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (sockaddr*) &name, &namelen);
    assert(err != -1);

    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, buflen);
    assert(p);

    close(sock);

    std::string ip_string = p;

    return LocalDirectory::IP_fromString(ip_string);

}
