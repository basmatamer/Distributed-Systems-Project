
#ifndef Message_hpp
#define Message_hpp

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "packet.hpp"
#include <vector>

enum MessageType {Request = 0, Reply = 1};
std::string typeToString(MessageType type);

class Message {

private:
    MessageType message_type;
    uint32_t message_id;

    std::string sender_username;
    uint32_t sender_ip;
    uint16_t sender_port;

    std::string receiver_username;
    uint32_t receiver_ip;
    uint16_t receiver_port;

    uint32_t operation_id;

    uint32_t arguments_size;
    char * arguments;

    void _unmarshal(char * marshalled_message);
    void _clear();

public:

    Message();
    Message(const Message& other);
    Message(Message&& other);
    Message(MessageType _type, uint32_t _id, uint32_t _operation);
    Message(MessageType _type, uint32_t _id, uint32_t _operation, uint32_t _data_size, char * _data);
    Message(MessageType _type, uint32_t _id, uint32_t _operation, uint32_t _data_size, char * _data, const std::string& _s_username, uint32_t _s_ip, uint16_t _s_port);
    Message(MessageType _type, uint32_t _id, uint32_t _operation, uint32_t _data_size, char * _data, const std::string& _s_username, uint32_t _s_ip, uint16_t _s_port, const std::string& _r_username, uint32_t _r_ip, uint16_t _r_port);

    Message(std::vector<packet>& packets);

    Message(char * marshalled_message);
    char * marshal (unsigned int& returned_size) const;

    ~Message();

    MessageType getMessageType() const;
    void setMessageType(MessageType t);

    uint32_t getMessageId() const;
    void setMessageId(uint32_t id);

    void getSender(std::string& _username, uint32_t& _ip, uint16_t& _port) const;
    void setSender(const std::string& _username, uint32_t _ip, uint16_t _port);
    void setSender(uint32_t _ip, uint16_t _port);

    std::string getReceiverUsername() const;
    uint32_t getSenderIP() const;
    uint16_t getSenderPort() const;

    void getReceiver(std::string& _username, uint32_t& _ip, uint16_t& _port) const;
    void setReceiver(const std::string& _username, uint32_t _ip, uint16_t _port);
    void setReceiver(uint32_t _ip, uint16_t _port);

    std::string getSenderUsername() const;
    uint32_t getReceiverIP() const;
    uint16_t getReceiverPort() const;

    uint32_t getOperation() const;
    void setOperation(uint32_t _op);

    uint32_t getArgumentsSize() const;
    uint32_t getArguments(char * buffer, uint32_t buffer_size) const;

    void setArguments(const char * _new_args, uint32_t size);

    uint32_t getTotalSize() const;

    void print() const;

    Message* createEmptyReply() const;
    Message* createReply(uint32_t reply_args_size, char * reply_args) const;

    void divide(std::vector<packet>& message_packets, uint32_t max_packet_size) const;
};

#endif /* Message_hpp */
