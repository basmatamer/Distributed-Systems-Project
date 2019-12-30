//  UDPEnhancedSocket.hpp
//  Distributed Project
//
//  Created by Mohamed on 11/6/18.
//  Copyright © 2018 Mohamed Tawfik. All rights reserved.
//

#ifndef UDPEnhancedSocket_hpp
#define UDPEnhancedSocket_hpp

#include "UDPSocket.hpp"
#include "Message.hpp"
#include "packet.hpp"
#include <map>
#include <vector>
#include "InterfaceDefintion.h"
#include <mutex>

enum PacketInsertResult{duplicate = 0, success = 1, message_completed = 2, failed = 3, ignored = 4};


class UDPEnhancedSocket: public UDPSocket {
protected:

    std::map<PacketKey, std::vector<packet> > packetsMap;

    static std::mutex mutex_serving_map;
    static std::map<uint32_t, uint32_t> currently_serving;

    PacketInsertResult addPacket(packet*);
    void insertPair(packet*);
    PacketInsertResult receivePacket(unsigned int buffer_size, PacketKey& packet_unique_id, uint32_t ack_timeout_usec, uint32_t& received_packet_number, uint32_t& received_packets_total);

    Message * receiveMessage(unsigned int buffer_size, uint32_t timeout_usec);

    bool sendWindow(const std::vector<char *>& flat_packets, const std::vector<uint32_t>& flat_packets_sizes, InternetAddress receiver_address, unsigned int start_from, unsigned int end_at_but_dont_send, uint32_t ack_timeout_usec, uint32_t resend_num);

    bool shouldSendAck(PacketInsertResult packet_insert_result, PacketKey packet_unique_id) const;
    int sendAck(packet * ack_on);
    bool receiveAck(packet * for_packet, uint32_t ack_timeout_usec);

    bool shouldInsert(packet * p);

public:
    UDPEnhancedSocket();
    UDPEnhancedSocket(uint32_t _myAddr, uint16_t _myPort);
    ~UDPEnhancedSocket();

    Message * sendMessage(Message * message, bool withGuarantee, uint32_t timeout_usec, uint32_t resend_num, uint32_t final_message_timeout);
};

#endif /* UDPEnhancedSocket_hpp */
