#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <vector>
#include <string>

// so assume the max size of data in a packet = 80 Bytes + 24 Bytes for other fields = 104 Bytes

struct PacketKey {
    uint32_t ip;
    uint16_t port;
    uint32_t message_id;

    bool operator==(const PacketKey& other) const{
        return (ip == other.ip && port == other.port && message_id == other.message_id);
    }

    bool operator<(const PacketKey& other) const{
        if (ip < other.ip) {
            return true;
        } else if (port < other.port) {
            return true;
        } else if (message_id < other.message_id) {
            return true;
        } else {
            return false;
        }
    }
};

class packet {

protected:
    uint32_t ip;   // IP address
    uint16_t port;  // port #
    uint32_t message_id;
    uint32_t number_of_packets;
    uint32_t packet_number;
    uint32_t data_size;
    char* data;

public:
    packet();
    packet(const packet& other);
    packet(packet&&);
    packet(uint32_t ip, uint16_t port, uint32_t message_id, uint32_t number_of_packets, uint32_t packet_number, uint32_t data_size, const char* data);
    packet(char * const flattened_packet);

    ~packet();

    char * getData(char * buffer, uint32_t buffer_size) const;
    char * getflatPacket(uint32_t& flat_packet_size) const;

    PacketKey getUniqueId() const;
    uint32_t getNumberofPackets() const;
    uint32_t getPacketNo() const;
    uint32_t getDataSize() const;

    void setSenderAddress(uint32_t sender_ip, uint16_t sender_port);

    void print() const;
    void print_packet_header() const;
    void print_message_id() const;
    void print_ip() const;
    void print_port() const;
    void print_number_of_packets() const;
    void print_packet_number() const;
    void print_data_size() const;
    void print_packet_data() const;

    bool operator < (const packet& other) const {
        return (packet_number < other.packet_number);
    }
    bool operator > (const packet& other) const {
        return (packet_number > other.packet_number);
    }

    void swap(packet& other);
    void swap(packet&& other);

    packet& operator=(packet&& other) {
        this->swap(other);
        return *this;
    }

    packet * getAckPacket(uint32_t ack_ip, uint16_t ack_port) const;
    bool isAckPacket() const;
    bool isAckPacketFor(packet * sent_packet) const;
    bool isAckPacketFor(char * const flattened_packet) const;

    uint32_t getDataCheckSum() const;

    static void debugPackets(const std::vector<packet>& packets);


};

int packetCompare(const packet& a, const packet& b);


#endif
