
#ifndef LocalDirectory_hpp
#define LocalDirectory_hpp

#include "InterfaceDefintion.h"
#include <string>
#include <map>
#include <vector>

class LocalDirectory {
protected:
    InternetAddress my_address;
    InternetAddress directoryService_address;
    std::map<std::string, InternetAddress> cached_addresses;

public:
    LocalDirectory();
    LocalDirectory(const LocalDirectory& other);
    LocalDirectory(LocalDirectory&& other);

    void setMyAddress(InternetAddress address);
    void setMyAddress(std::string ip_address, uint16_t port);
    void setMyAddress(uint32_t ip_address, uint16_t port);
    InternetAddress getMyAddress() const;
    uint32_t getMyIP() const;
    uint16_t getMyPort() const;

    void setDirectoryAddress(InternetAddress address);
    void setDirectoryAddress(std::string ip_address, uint16_t port);
    void setDirectoryAddress(uint32_t ip_address, uint16_t port);
    InternetAddress getDirectoryAddress() const;
    uint32_t getDirectoryIP() const;
    uint16_t getDirectoryPort() const;

    void setPeerAddress(const std::string& username, InternetAddress address);
    void setPeerAddress(const std::string& username, std::string ip_address, uint16_t  port);
    void setPeerAddress(const std::string& username, uint32_t ip_address, uint16_t port);
    InternetAddress getPeerAddress(const std::string& username) const;
    uint32_t getPeerIP(const std::string& username) const;
    uint16_t getPeerPort(const std::string& username) const;

    std::vector<std::string> getAllPeerUsernames() const;

    static std::string IP_toString(uint32_t ip);
    static uint32_t IP_fromString(const std::string& ip_string);
    static InternetAddress getEmptyAddress();

    static uint32_t GetPrimaryIp();
};

#endif /* LocalDirectory_hpp */
