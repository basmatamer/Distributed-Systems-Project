#include "Message.hpp"
#include "ErrorHandler.hpp"
#include "Converter.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "packet.hpp"
#include <vector>
#include "InterfaceDefintion.h"
#include <cmath>
#include "LocalDirectory.hpp"

std::string typeToString(MessageType type) {
    switch (type) {
        case Request:
            return "Request";
        case Reply:
            return "Reply";
    }
}

Message::Message() {
    _clear();
}
Message::Message(const Message& other):message_type(other.message_type), message_id(other.message_id), sender_username(other.sender_username), sender_ip(other.sender_ip), sender_port(other.sender_port), receiver_username(other.receiver_username), receiver_ip(other.receiver_ip), operation_id(other.operation_id), arguments_size(other.arguments_size) {
    if(arguments_size < 1 || other.arguments == NULL) {
        arguments_size = 0;
        arguments = NULL;
    } else {
        arguments = new char [arguments_size];
        ErrorHandler::checkPointer(arguments, "Memory allocation while copying a message failed");
        for(unsigned int i  = 0; i < arguments_size; i++) {
            arguments[i] = other.arguments[i];
        }
    }
}
Message::Message(Message&& other):message_type(other.message_type), message_id(other.message_id), sender_username(other.sender_username), sender_ip(other.sender_ip), sender_port(other.sender_port), receiver_username(other.receiver_username), receiver_ip(other.receiver_ip), operation_id(other.operation_id), arguments_size(other.arguments_size), arguments(other.arguments) {
    other.arguments = NULL;
}
Message::Message(MessageType _type, uint32_t _id, uint32_t _operation) {
    this->message_type = _type;
    this->message_id = _id;
    this->sender_username = "";
    this->sender_ip = 0;
    this->sender_port = 0;
    this->receiver_username = "";
    this->receiver_ip = 0;
    this->receiver_port = 0;
    this->operation_id = _operation;
    this->arguments_size = 0;
    this->arguments = NULL;
}
Message::Message(MessageType _type, uint32_t _id, uint32_t _operation, uint32_t _data_size, char * _data) {
    this->message_type = _type;
    this->message_id = _id;
    this->sender_username = "";
    this->sender_ip = 0;
    this->sender_port = 0;
    this->receiver_username = "";
    this->receiver_ip = 0;
    this->receiver_port = 0;
    this->operation_id = _operation;
    this->arguments_size = _data_size;
    if(_data_size > 0) {
        this->arguments = new char[_data_size];
        ErrorHandler::checkPointer(this->arguments, "Memory allocation while creating message failed");
        for(unsigned int i = 0; i < this->arguments_size; i++) {
            arguments[i] = _data[i];
        }
    } else {
        this->arguments = NULL;
    }
}
Message::Message(MessageType _type, uint32_t _id, uint32_t _operation, uint32_t _data_size, char * _data, const std::string& _s_username, uint32_t _s_ip, uint16_t _s_port) {
    this->message_type = _type;
    this->message_id = _id;
    this->sender_username = _s_username;
    this->sender_ip = _s_ip;
    this->sender_port = _s_port;
    this->receiver_username = "";
    this->receiver_ip = 0;
    this->receiver_port = 0;
    this->operation_id = _operation;
    this->arguments_size = _data_size;
    if(_data_size > 0) {
        this->arguments = new char[_data_size];
        ErrorHandler::checkPointer(this->arguments, "Memory allocation while creating message failed");
        for(unsigned int i = 0; i < this->arguments_size; i++) {
            arguments[i] = _data[i];
        }
    } else {
        this->arguments = NULL;
    }
}
Message::Message(MessageType _type, uint32_t _id, uint32_t _operation, uint32_t _data_size, char * _data, const std::string& _s_username, uint32_t _s_ip, uint16_t _s_port, const std::string& _r_username, uint32_t _r_ip, uint16_t _r_port) {
    this->message_type = _type;
    this->message_id = _id;
    this->sender_username = _s_username;
    this->sender_ip = _s_ip;
    this->sender_port = _s_port;
    this->receiver_username = _r_username;
    this->receiver_ip = _r_ip;
    this->receiver_port = _r_port;
    this->operation_id = _operation;
    this->arguments_size = _data_size;
    if(_data_size > 0) {
        this->arguments = new char[_data_size];
        ErrorHandler::checkPointer(this->arguments, "Memory allocation while creating message failed");
        for(unsigned int i = 0; i < this->arguments_size; i++) {
            arguments[i] = _data[i];
        }
    } else {
        this->arguments = NULL;
    }
}

Message::Message(char * marshalled_message) {
    _unmarshal(marshalled_message);
}

Message::Message(std::vector<packet>& packets) {
    _clear();
    unsigned int total_size = 0;

    if(packets.size() > 10) {
         std::cout << "Debugging the received packets before sorting" << std::endl;

         packet::debugPackets(packets);
    }


    if(packets.size() > 1) {
        std::cout << "Attempting to sort the " << packets.size() << " packets in message class" << std::endl;

        std::sort(packets.begin(), packets.end(), packetCompare);

        /*
        unsigned int n = packets.size();
        for(unsigned int i = 0; i < n - 1; i++) {
            unsigned int min_index = i;
            for(unsigned int j = i+1; j < n; j++) {
                if(packets[j] < packets[min_index]) {
                    min_index = j;
                }
            }
            if(min_index != i) {
                packets[i].swap(packets[min_index]);
            }
        }
        */

    }

    if(packets.size() > 10) {
         std::cout << "Debugging the received packets after sorting" << std::endl;

        packet::debugPackets(packets);
    }


    for(unsigned int i = 0; i < packets.size(); i++) {
        total_size += packets[i].getDataSize();
    }

    if(total_size < 40) {
        ErrorHandler::die(std::string("This is a corrupt message. Only ") + std::to_string(total_size) + std::string(" bytes were received "));
        return;
    }

    char * marshalled_message = new char[total_size];
    ErrorHandler::checkPointer(marshalled_message, "Memory allocation while combining packets into a message failed");

    char * pointer = marshalled_message;
    for(unsigned int i = 0; i < packets.size(); i++) {
        packets[i].getData(pointer, packets[i].getDataSize());
        pointer += packets[i].getDataSize();
    }

    _unmarshal(marshalled_message);

    PacketKey packet_id = packets[0].getUniqueId();
    sender_ip = packet_id.ip;
    sender_port = packet_id.port;

    if(marshalled_message != NULL) {
        delete [] marshalled_message;
        marshalled_message = NULL;
        pointer = NULL;
    }
}

Message::~Message() {
    if(arguments != NULL) {
        delete [] arguments;
        arguments = NULL;
    }
}

void Message::_clear() {
    this->message_type = Request;
    this->message_id = 0;
    this->sender_username = "";
    this->sender_ip = 0;
    this->sender_port = 0;
    this->receiver_username = "";
    this->receiver_ip = 0;
    this->receiver_port = 0;
    this->operation_id = 0;
    this->arguments_size = 0;
    this->arguments = NULL;
}

char * Message::marshal(unsigned int& returned_size) const {
    returned_size = 0;

    uint32_t sender_username_length = uint32_t(sender_username.length());
    uint32_t receiver_username_length = uint32_t(receiver_username.length());

 //   std::cout << "Attempting to marshal a message" << std::endl;
 //   std::cout << "Sender username length: " << sender_username_length << std::endl;
 //   std::cout << "Receiver username length: " << receiver_username_length << std::endl;
 //   std::cout << "Arguments size: " << arguments_size << std::endl;


    uint32_t marshalled_size = getTotalSize();

    char * marshalled_message = new char [marshalled_size];
    ErrorHandler::checkPointer(marshalled_message, "Memory Allocation for marshalling a message failed");
    returned_size = marshalled_size;
 //   std::cout << "Returned size: " << returned_size << std::endl;

    char * pointer = marshalled_message;

    Converter::int_to_chars(uint32_t(message_type), pointer);
    pointer += 4;

    Converter::int_to_chars(message_id, pointer);
    pointer += 4;

    Converter::int_to_chars(sender_username_length, pointer);
    pointer += 4;

    Converter::string_to_chars(sender_username, pointer);
    pointer += sender_username_length;

    Converter::int_to_chars(sender_ip, pointer);
    pointer += 4;

    Converter::short_to_chars(sender_port, pointer);
    pointer += 2;

    Converter::int_to_chars(receiver_username_length, pointer);
    pointer += 4;

    Converter::string_to_chars(receiver_username, pointer);
    pointer += receiver_username_length;

    Converter::int_to_chars(receiver_ip, pointer);
    pointer += 4;

    Converter::short_to_chars(receiver_port, pointer);
    pointer += 2;

    Converter::int_to_chars(operation_id, pointer);
    pointer += 4;

    Converter::int_to_chars(arguments_size, pointer);
    pointer += 4;

    for(unsigned int i = 0; i < arguments_size; i++) {
        pointer[i] = arguments[i];
    }
    pointer += arguments_size;

    return marshalled_message;
}
void Message::_unmarshal(char * marshalled_message) {
    char * pointer = marshalled_message;

    this->message_type = MessageType((int(Converter::chars_to_int(pointer))));
    pointer += 4;

    this->message_id = Converter::chars_to_int(pointer);
    pointer += 4;

    uint32_t sender_username_length = Converter::chars_to_int(pointer);
    pointer += 4;

    this->sender_username = Converter::chars_to_string(sender_username_length, pointer);
    pointer += sender_username_length;

    this->sender_ip = Converter::chars_to_int(pointer);
    pointer += 4;

    this->sender_port = Converter::chars_to_short(pointer);
    pointer += 2;

    int receiver_username_length = Converter::chars_to_int(pointer);
    pointer += 4;

    this->receiver_username = Converter::chars_to_string(receiver_username_length, pointer);
    pointer += receiver_username_length;

    this->receiver_ip = Converter::chars_to_int(pointer);
    pointer += 4;

    this->receiver_port = Converter::chars_to_short(pointer);
    pointer += 2;

    this->operation_id = Converter::chars_to_int(pointer);
    pointer += 4;

    this->arguments_size = Converter::chars_to_int(pointer);
    pointer += 4;

    if(arguments_size > 0) {
        this->arguments = new char[arguments_size];
        ErrorHandler::checkPointer(this->arguments, "Memory allocation while unmarshalling message failed");

        for(unsigned int i = 0; i < arguments_size; i++) {
            this->arguments[i] = pointer[i];
        }
        pointer += this->arguments_size;
    } else {
        this->arguments = NULL;
    }
}

MessageType Message::getMessageType() const {
    return message_type;
}
void Message::setMessageType(MessageType t) {
    message_type = t;
}

uint32_t Message::getMessageId() const {
    return message_id;
}
void Message::setMessageId(uint32_t id) {
    message_id = id;
}

void Message::setSender(const std::string& _username, uint32_t _ip, uint16_t _port) {
    sender_username = _username;
    sender_ip = _ip;
    sender_port = _port;
}
void Message::setSender(uint32_t _ip, uint16_t _port) {
    sender_ip = _ip;
    sender_port = _port;
}
void Message::getSender(std::string& _username, uint32_t& _ip, uint16_t& _port) const {
    _username = sender_username;
    _ip = sender_ip;
    _port = sender_port;
}
uint32_t Message::getSenderIP() const {
    return sender_ip;
}
uint16_t Message::getSenderPort() const {
    return sender_port;
}

void Message::setReceiver(const std::string& _username, uint32_t _ip, uint16_t _port) {
    receiver_username = _username;
    receiver_ip = _ip;
    receiver_port = _port;
}
void Message::setReceiver(uint32_t _ip, uint16_t _port) {
    receiver_ip = _ip;
    receiver_port = _port;
}

void Message::getReceiver(std::string& _username, uint32_t& _ip, uint16_t& _port) const {
    _username = receiver_username;
    _ip = receiver_ip;
    _port = receiver_port;
}


uint32_t Message::getReceiverIP() const {
    return receiver_ip;
}
uint16_t Message::getReceiverPort() const {
    return receiver_port;
}

uint32_t Message::getOperation() const {
    return operation_id;
}
void Message::setOperation(uint32_t _op) {
    operation_id = _op;
}

uint32_t Message::getArgumentsSize() const {
    return arguments_size;
}
uint32_t Message::getArguments(char * buffer, uint32_t buffer_size) const {
    if (arguments == NULL || buffer == NULL) {
        return 0;
    }
    uint32_t minimum = std::min(buffer_size, arguments_size);
    for(int i = 0; i < minimum; i++) {
        buffer[i] = arguments[i];
    }
    return minimum;
}

void Message::setArguments(const char * _new_args, uint32_t _size) {
    if(_new_args == NULL || _size < 1) {
        return;
    }
    if(arguments != NULL) {
        delete [] arguments;
        arguments = NULL;
    }
    arguments = new char[_size];
    ErrorHandler::checkPointer(arguments, "Memory allocation for new arguments failed");
    for(int i = 0; i < _size; i++) {
        arguments[i] = _new_args[i];
    }
    arguments_size = _size;
}

uint32_t Message::getTotalSize() const {
    return 36 + arguments_size + uint32_t(sender_username.length()) + uint32_t(receiver_username.length());
}

void Message::print() const {
    std::cout << "Printing message " << message_id << " .." << std::endl;
    std::cout << "Type: " << typeToString(message_type) << std::endl;
    std::cout << "Sender: " << sender_username << " " << LocalDirectory::IP_toString(sender_ip) << " " << sender_port << std::endl;
    std::cout << "Receiver: " << receiver_username << " " << LocalDirectory::IP_toString(receiver_ip) << " " << std::dec << receiver_port << std::endl;
    std::cout << "Operation ID: " << operation_id << std::endl;
    std::cout << "Arguments Size: " << arguments_size << std::endl;
//    std::cout << "Arguments:" << std::endl;
//    if(arguments == NULL) {
//        std::cout << "No arguments exist" << std::endl;
//    } else {
//        for(unsigned int i = 0; i < arguments_size; i++) {
//            std::cout << "Byte #" << i << ": 0x" << std::hex << (unsigned int)(unsigned char)(arguments[i]) << std::dec << std::endl;
//        }
//    }
}

Message* Message::createEmptyReply() const {
    return new Message(Reply, message_id, OperationID::EMPTY, 0, NULL, receiver_username, receiver_ip, receiver_port, sender_username, sender_ip, sender_port);
}

Message* Message::createReply(uint32_t reply_args_size, char* reply_args) const {
    return new Message(Reply, message_id, operation_id, reply_args_size, reply_args, receiver_username, receiver_ip, receiver_port, sender_username, sender_ip, sender_port);
}


// the target of this function is to divide a flat message to an array of packets !!!!!
void Message::divide(std::vector<packet>& message_packets, uint32_t max_packet_size) const {
    // size of 1 packet = 24Bytes + 80Bytes of actual data = 104Bytes
    unsigned int number_of_packets;
    unsigned int flat_message_size;
    const char * flat_message = marshal(flat_message_size);
    message_packets.resize(0);

    // std::cout << "The message id in marshal::divide is " << message_id << std::endl;

    char* packet_data = new char[max_packet_size];
    ErrorHandler::checkPointer(packet_data, "Memory allocation while converting from message to packets failed");

    if (flat_message_size <= max_packet_size) {
        number_of_packets = 1;
        packet my_packet = packet(sender_ip, sender_port, message_id, 1, 0, flat_message_size, flat_message);
        message_packets.push_back(my_packet);
    }

    else {
        number_of_packets = std::ceil(flat_message_size / float(max_packet_size));
        unsigned int packet_size = max_packet_size;
        // will have a loop for number_of_packets
        int i = 0;
        while (i < number_of_packets) {
            if (i == number_of_packets - 1) {
                packet_size = flat_message_size - (number_of_packets - 1) * max_packet_size;
            }

            for (unsigned int j = 0; j < packet_size; j++)
                packet_data[j] = flat_message[i*max_packet_size+j];

            packet my_packet = packet(sender_ip, sender_port, message_id, number_of_packets, i, packet_size, packet_data);
            message_packets.push_back(my_packet);
            i++;
        }
    }

    delete [] packet_data;
    packet_data = NULL;
}

std::string Message::getSenderUsername() const {
    return sender_username;
}

std::string Message::getReceiverUsername() const {
    return receiver_username;
}
