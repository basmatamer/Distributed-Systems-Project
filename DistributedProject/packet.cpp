#include <arpa/inet.h>
#include <algorithm>

#include "packet.hpp"

#include "ErrorHandler.hpp"
#include "Converter.hpp"
#include "InterfaceDefintion.h"
#include "LocalDirectory.hpp"

#include "ourexception.h"

// Default constructor
packet::packet() {
    this->packet_number = 0;
    this->number_of_packets = 0;
    this->ip = 0;
    this->port = 0;
    this->message_id = 0;
    this->data_size = 0;

    this->data = NULL;
}
packet::packet(const packet& other):packet_number(other.packet_number), number_of_packets(other.number_of_packets), ip(other.ip), port(other.port), message_id(other.message_id), data_size(other.data_size) {
    if(data_size < 1 || other.data == NULL) {
        data = NULL;
    } else {
        data = new char [data_size];
        ErrorHandler::checkPointer(data, "Memory allocation while copying packet failed");
        for(unsigned int i = 0; i < data_size; i++) {
            data[i] = other.data[i];
        }
    }
}
packet::packet(packet&& other):packet_number(other.packet_number), number_of_packets(other.number_of_packets), ip(other.ip), port(other.port), message_id(other.message_id), data_size(other.data_size), data(other.data) {
    other.data = NULL;
}

packet::packet(uint32_t ip, uint16_t port, uint32_t message_id, uint32_t number_of_packets, uint32_t packet_number, uint32_t data_size, const char* data) {
    this->ip = ip;
    this->port = port;
    this->message_id = message_id;
    this->number_of_packets = number_of_packets;
    this->packet_number = packet_number;
    this->data_size = data_size;

    this->data = new char[data_size];
    ErrorHandler::checkPointer(this->data, "Memory allocation for packet failed");
    for (int i = 0; i < data_size; i++)
        this->data[i] = data[i];
}

// Unmarshall a flattened packet
packet::packet(char * const flat_packet) {
    char * pointer = flat_packet;

    uint32_t flat_packet_size = Converter::chars_to_int(pointer);
    pointer += 4;

    this->ip = Converter::chars_to_int(pointer);
    pointer += 4;

    this->port = Converter::chars_to_short(pointer);
    pointer += 2;

    this->message_id = Converter::chars_to_int(pointer);
    pointer += 4;

    this->number_of_packets = Converter::chars_to_int(pointer);
    pointer += 4;

    this->packet_number = Converter::chars_to_int(pointer);
    pointer += 4;

    this->data_size = Converter::chars_to_int(pointer);
    pointer += 4;

    this->data = new char[data_size];
    ErrorHandler::checkPointer(this->data, "Memory allocation for packet failed");
    for(unsigned int i = 0; i < this->data_size; i++) {
        this->data[i] = pointer[i];
    }

    if(flat_packet_size != 26 + this->data_size) {
        ErrorHandler::displayWarning("Corrupt packet");
    }
}

// Destructor
packet::~packet() {
    if(this->data != NULL) {
        delete [] this->data;
        this->data = NULL;
    }
}

char * packet::getData(char * buffer, uint32_t buffer_size) const {
    uint32_t minimum = std::min(buffer_size, data_size);
    if(this->data == NULL || buffer == NULL) {
        throw PacketDataNullPointerException(std::string("Attempting to use a null pointer while returning packet data into a buffer")
                                             );
    }
    if(minimum < data_size) {
        ErrorHandler::displayWarning("Buffer to retreive packet data into is too small");
    }
    for(unsigned int i = 0; i < minimum; i++) {
        buffer[i] = data[i];
    }
    return buffer;
}

uint32_t packet::getDataSize() const {
    return (this->data_size);
}

// Marshall the packet
char* packet::getflatPacket(uint32_t& flat_packet_size) const {
    // Note that 24 comes from 32+32+32+32+32+32=192 / 8 = 24Bytes for all the above integer fields
    flat_packet_size = getDataSize() + 26;

    char* flat_packet = new char[flat_packet_size];
    ErrorHandler::checkPointer(flat_packet, "Memory allocation for flat packet failed");

    char* pointer = flat_packet;

    Converter::int_to_chars(flat_packet_size, pointer);
    pointer += 4;

    Converter::int_to_chars((this->ip), pointer);
    pointer += 4;

    Converter::short_to_chars((this->port), pointer);
    pointer += 2;

    Converter::int_to_chars((this->message_id), pointer);
    pointer += 4;

    Converter::int_to_chars((this->number_of_packets), pointer);
    pointer += 4;

    Converter::int_to_chars((this->packet_number), pointer);
    pointer += 4;

    Converter::int_to_chars((this->data_size), pointer);
    pointer += 4;

    for(unsigned int i = 0; i < this->data_size; i++) {
        pointer[i] = this->data[i];
    }

    return flat_packet;
}

void packet::setSenderAddress(uint32_t sender_ip, uint16_t sender_port) {
    ip = sender_ip;
    port = sender_port;
}

void packet::print() const {
    std::cout << "Printing Packet .." << std::endl;
    print_packet_header();
    print_packet_data();
}

void packet::print_packet_header() const {
    print_ip();
    print_port();
    print_message_id();
    print_number_of_packets();
    print_packet_number();
    print_data_size();
}

void packet::print_packet_number() const {
    std::cout << "Packet number: " << packet_number << std::endl;
}
void packet::print_number_of_packets() const {
    std::cout << "Number of Packets: " << number_of_packets << std::endl;

}
void packet::print_ip() const {
    std::cout << "Sender IP: 0x" << std::hex << ip << std::dec << std::endl;

}
void packet::print_port() const {
    std::cout << "Sender Port: " << port << std::endl;
}
void packet::print_message_id() const {
    std::cout << "Message id: " << message_id << std::endl;
}
void packet::print_data_size() const {
    std::cout << "Data size: " << data_size << std::endl;
}
void packet::print_packet_data() const {
    if(data_size < 200)
    {
        std::cout << "The data in packet is: " << std::endl;
        for(unsigned int i = 0; i < data_size; i++) {
            std::cout << "Byte #" << i << ": 0x" << std::hex << (unsigned int)(unsigned char)(data[i]) << std::dec << std::endl;
        }
        std::cout << std::endl;
    }

}

PacketKey packet::getUniqueId() const {
    PacketKey key;
    key.ip = ip;
    key.port = port;
    key.message_id = message_id;
    return key;
}

uint32_t packet::getNumberofPackets() const {
    return number_of_packets;
}

uint32_t packet::getPacketNo() const {
    return packet_number;
}

void packet::swap(packet& other) {
    std::swap(ip, other.ip);
    std::swap(port, other.port);
    std::swap(message_id, other.message_id);
    std::swap(packet_number, other.packet_number);
    std::swap(number_of_packets, other.number_of_packets);
    std::swap(data, other.data);
    std::swap(data_size, other.data_size);
}
void packet::swap(packet&& other) {
    std::swap(ip, other.ip);
    std::swap(port, other.port);
    std::swap(message_id, other.message_id);
    std::swap(packet_number, other.packet_number);
    std::swap(number_of_packets, other.number_of_packets);
    std::swap(data, other.data);
    std::swap(data_size, other.data_size);
}

packet * packet::getAckPacket(uint32_t ack_ip, uint16_t ack_port) const {
    uint32_t ack_size = 9;
    char * ack_args = new char[ack_size];
    ErrorHandler::checkPointer(ack_args, "Memory allocation while creating ack args failed");

    char * pointer = ack_args;

    Converter::bool_to_chars(true, pointer);
    pointer += 1;

    Converter::int_to_chars(message_id, pointer);
    pointer += 4;

    Converter::int_to_chars(packet_number, pointer);
    pointer += 4;

    packet * ack_packet = new packet(ack_ip, ack_port, 0xFFFFFFFF, 1, 0, ack_size, ack_args);

    ErrorHandler::checkPointer(ack_packet, "Memory allocation while creating ack packet failed");
    delete [] ack_args;

    return ack_packet;
}

bool packet::isAckPacket() const {
    if(message_id != 0xFFFFFFFF || number_of_packets != 1 || packet_number != 0) {
        return false;
    }

    if(data_size != 9) {
        return false;
    }

    char * pointer = data;

    bool isAck = Converter::chars_to_bool(pointer);
    pointer += 1;
    return isAck;
}

bool packet::isAckPacketFor(packet * sent_packet) const {
    if(!isAckPacket()) {
        std::cout << "The problem is that this is not an ack packet" << std::endl;
        return false;
    }
    char * pointer = data;
    pointer += 1;

    uint32_t sender_message_id = Converter::chars_to_int(pointer);
    pointer += 4;
    if(sender_message_id != sent_packet->message_id) {
        std::cout << "The problem is that the message ids are incorrect" << std::endl;
        std::cout << sender_message_id << " " << sent_packet->message_id << std::endl;
        return false;
    }

    uint32_t sender_packet_number = Converter::chars_to_int(pointer);
    pointer += 4;
    if(!(sender_packet_number / WINDOW_SIZE) == (sent_packet->packet_number / WINDOW_SIZE)) {
        std::cout << "The problem are with the windows" << std::endl;
        std::cout << sender_packet_number << " " << sent_packet->packet_number;
        std::cout << sender_packet_number / WINDOW_SIZE << " " << sent_packet->packet_number / WINDOW_SIZE;
        return false;
    } else {
        return true;
    }
}

bool packet::isAckPacketFor(char * const flattened_packet) const {
    packet * sent_packet = new packet(flattened_packet);
    bool result = isAckPacketFor(sent_packet);
    delete sent_packet;
    return result;
}

uint32_t packet::getDataCheckSum() const {
    uint32_t sum = 0;
    for(unsigned int i = 0; i < data_size; i++) {
        sum += data[i] % 0xFFFFFF;
    }
    return sum;
}

void packet::debugPackets(const std::vector<packet>& packets)
{
    for(unsigned int i = 0; i < packets.size(); i++) {
        PacketKey packet_key = packets[i].getUniqueId();
        std::cout << "Packet of index " << i << " has message id " << packet_key.message_id;
        std::cout << " and is packet " << packets[i].getPacketNo() << "/" << packets[i].getNumberofPackets();
        std::cout << " and has " << packets[i].getDataSize() << " data bytes with checksum " << packets[i].getDataCheckSum();
        std::cout << std::endl;

    }
}

int packetCompare(const packet& a, const packet& b) {
    return a < b;
}
