//
//  UDPEnhancedSocket.cpp
//  Distributed Project
//
//  Created by Mohamed on 11/6/18.
//  Copyright © 2018 Mohamed Tawfik. All rights reserved.
//

#include "UDPEnhancedSocket.hpp"
#include "Message.hpp"
#include "packet.hpp"
#include "InterfaceDefintion.h"
#include "ErrorHandler.hpp"
#include "LocalDirectory.hpp"

#include <time.h>
#include <algorithm>
#include <vector>
#include <mutex>

std::mutex UDPEnhancedSocket::mutex_serving_map;
std::map<uint32_t, uint32_t> UDPEnhancedSocket::currently_serving;

UDPEnhancedSocket::UDPEnhancedSocket():UDPSocket() {
}
UDPEnhancedSocket::UDPEnhancedSocket(uint32_t _myAddr, uint16_t _myPort):UDPSocket(_myAddr, _myPort) {
}
UDPEnhancedSocket::~UDPEnhancedSocket() {
}

Message * UDPEnhancedSocket::sendMessage(Message * message, bool withGuarantee, uint32_t ack_timeout_usec, uint32_t resend_num, uint32_t final_message_timeout) {
    Message * reply_message = NULL;

    message->setSender(getMyAddress(), getMyPort());

    if(message->getMessageType() == Reply) {
        mutex_serving_map.lock();
        std::lock_guard<std::mutex> lk_serving_map(mutex_serving_map, std::adopt_lock);

        currently_serving.erase(message->getReceiverIP());
    }

    std::vector<packet> message_packets;
    message_packets.resize(0);

    message->divide(message_packets, PACKET_DATA_SIZE);

    if(message_packets.size() > 10) {
        std::cout << "Debugging the packets befpre sending them" << std::endl;
        packet::debugPackets(message_packets);
    }

    std::vector<char*> flat_packets;
    std::vector<uint32_t> flat_packets_sizes;

    flat_packets.resize(0);
    flat_packets_sizes.resize(0);

    for(unsigned int i = 0; i < message_packets.size(); i++) {
        uint32_t flat_packet_size = 0;
        char * new_flat_packet = message_packets[i].getflatPacket(flat_packet_size);

        flat_packets.push_back(new_flat_packet);
        flat_packets_sizes.push_back(flat_packet_size);
    }

    message_packets.clear();

//    std::cout << "Attempting to send " << flat_packets.size() << " packets" << std::endl;

    InternetAddress receiver_address;
    receiver_address.ip = message->getReceiverIP();
    receiver_address.port = message->getReceiverPort();

    unsigned int windows_num = flat_packets.size() / WINDOW_SIZE;

//    std::cout << "Number of windows is " << windows_num << std::endl;
    for(unsigned int i = 0; i < windows_num; i++) {
        bool window_result = sendWindow(flat_packets, flat_packets_sizes, receiver_address, i * WINDOW_SIZE, (i+1) * WINDOW_SIZE, ack_timeout_usec, resend_num);
        if(!window_result) {
            // No ack was received for this window
            // Abort
            std::cout << "No ack was received for window " << i << "/" << windows_num << std::endl;
            for(unsigned int k = 0; k < flat_packets.size(); k++) {
                if(flat_packets[k] != NULL) {
                    delete [] flat_packets[k];
                    flat_packets[k] = NULL;
                }
            }
            flat_packets.clear();
            flat_packets_sizes.clear();
            return NULL;
        }
    }

    int remainder_packets = flat_packets.size() - (windows_num * WINDOW_SIZE);

    if(remainder_packets > 0) {
        std::cout << "Sending the remainder " << remainder_packets << " packets" << std::endl;
        bool remainder_result = sendWindow(flat_packets, flat_packets_sizes, receiver_address, windows_num * WINDOW_SIZE, flat_packets.size(), ack_timeout_usec, resend_num);
        if(!remainder_result) {
            // No ack was received for this window
            // Abort
            std::cout << "No ack was received for the " << flat_packets.size() - (windows_num * WINDOW_SIZE) << " remainder packets" << std::endl;
            for(unsigned int k = 0; k < flat_packets.size(); k++) {
                if(flat_packets[k] != NULL) {
                    delete [] flat_packets[k];
                    flat_packets[k] = NULL;
                }
            }
            flat_packets.clear();
            flat_packets_sizes.clear();
            return NULL;
        }
    }

    for(unsigned int i = 0; i < flat_packets.size(); i++) {
        delete [] flat_packets[i];
        flat_packets[i] = NULL;
    }
    flat_packets.clear();
    flat_packets_sizes.clear();


    if(withGuarantee) {
        reply_message = receiveMessage(BUFFER_SIZE, final_message_timeout);
    }

    return reply_message;    // will return the reply of server as a Message
}

Message * UDPEnhancedSocket::receiveMessage(unsigned int buffer_size, uint32_t timeout_usec) {
    PacketKey packet_unique_id;

    Message * new_message = NULL;
    PacketInsertResult packet_insert_result;
    uint32_t received_packet_number;
    uint32_t received_packets_total;

    while(true) {
        received_packet_number = 0;
        received_packets_total = 0;
        packet_insert_result = receivePacket(buffer_size, packet_unique_id, timeout_usec, received_packet_number, received_packets_total);

        // ack or not
        if(shouldSendAck(packet_insert_result, packet_unique_id)) {
            packet * ack_on = &(packetsMap[packet_unique_id][packetsMap[packet_unique_id].size()-1]);
            sendAck(ack_on);
        }

        switch(packet_insert_result) {
            case duplicate:
                // a duplicate packet was received. Just ignore it.
                std::cout << "Packet " << received_packet_number << "/" << received_packets_total;
                std::cout << " from " << LocalDirectory::IP_toString(packet_unique_id.ip) << " with port number " << packet_unique_id.port;
                std::cout << " of message " << packet_unique_id.message_id << " is a duplicate" << std::endl;
                break;
            case success:
                // do nothing. Still need more packets.
                break;
            case message_completed:
                // a message is completed. Return it to above layer to handle it.
                std::cout << "Message completed" << std::endl;

                new_message = new Message(packetsMap[packet_unique_id]);
                ErrorHandler::checkPointer(new_message, "Memory allocation for incoming message failed");
                // NOTE
                // Added this as an extra check to see if a message is "corrupted"

                if(new_message->getSenderUsername() == "") {
                    ErrorHandler::displayWarning("Printing corrupted message");
                    new_message->print();
                    delete new_message;
                    new_message = NULL;
                }

                std::cout << "A message has just completely arrived" << std::endl;
                packetsMap.erase(packet_unique_id);
                return new_message;
                break;

            case failed:
                // TODO check this later
                ErrorHandler::displayWarning("Attempt to receive a packet has failed.\nWhat should we do now?");
                return NULL;
                break;

            case ignored:
                // do nothing
                // This probably was an ack packet that should not have arrived here
                // Ignore it
                break;
        }
    }
}

PacketInsertResult UDPEnhancedSocket::receivePacket(unsigned int buffer_size, PacketKey& packet_unique_id, uint32_t timeout_usec, uint32_t& received_packet_number, uint32_t& received_packets_total) {
    char * buffer = new char[buffer_size];
    ErrorHandler::checkPointer(buffer, "Memory allocation for receiving a packet failed");

    packet_unique_id.ip = 0;
    packet_unique_id.port = 0;
    packet_unique_id.message_id = 0;
    received_packet_number = 0;
    received_packets_total = 0;

    int len = UDPSocket::_receiveWithTimeout(buffer, buffer_size, packet_unique_id.ip, packet_unique_id.port, timeout_usec);
    if(len == -1) {
        return failed;
    }

    packet * p = new packet(buffer);
    ErrorHandler::checkPointer(p, "Memory allocation for incoming packet failed");
    delete [] buffer;
    buffer = NULL;

    p->setSenderAddress(packet_unique_id.ip, packet_unique_id.port);
    packet_unique_id = p->getUniqueId();
    received_packet_number = p->getPacketNo();
    received_packets_total = p->getNumberofPackets();

    // If this is an ack packet, ignore it
    if(p->isAckPacket()) {
        delete p;
        p = NULL;
        return ignored;
    }

    //  std::cout << "Attempting to add packet" << std::endl;
    return addPacket(p);
}

PacketInsertResult UDPEnhancedSocket::addPacket(packet* p) {
    PacketKey packet_id = p->getUniqueId();
    uint32_t packets_num = p->getNumberofPackets();

    if(!packetsMap.empty()) {
        if(packetsMap.find(packet_id) != packetsMap.end()) {
            for(unsigned int i = 0; i < packetsMap[packet_id].size(); i++) {
                if(p->getPacketNo() == packetsMap[packet_id][i].getPacketNo()) {
                    // This packet was inserted before. No need to insert it again.
                    delete p;
                    return duplicate;
                }
            }
            packetsMap[packet_id].push_back(*p);
            delete p;
        } else {
            insertPair(p);
        }
    } else {
        insertPair(p);
    }

    if(packetsMap[packet_id].size() == packets_num) {
        return message_completed;
    } else {
        return success;
    }
}

void UDPEnhancedSocket::insertPair(packet* p) {
    std::vector<packet> temp;
    temp.resize(0);
    temp.push_back(*p);
    packetsMap.insert(std::make_pair(p->getUniqueId(), temp));
    delete p;
}

bool UDPEnhancedSocket::shouldSendAck(PacketInsertResult packet_insert_result, PacketKey packet_unique_id) const {
    try {
        if(packet_insert_result == message_completed) {
            return true;
        } else if(packet_insert_result == failed || packet_insert_result == ignored) {
            return false;
        } else if(packetsMap.at(packet_unique_id).size() % WINDOW_SIZE == 0) {
            return true;
        } else {
            return false;
        }
    } catch(...) {
        ErrorHandler::displayWarning("Attempting to check if should send ack caused exception");
        return false;
    }
}



int UDPEnhancedSocket::sendAck(packet * ack_on) {
    PacketKey packet_unique_id = ack_on->getUniqueId();
    packet * ack_packet = ack_on->getAckPacket(getMyAddress(), getMyPort());

//    std::cout << "Attempting to send an acklowedgment to " << LocalDirectory::IP_toString(packet_unique_id.ip) << " at " << packet_unique_id.port;
//    std::cout << " for message " << packet_unique_id.message_id;
//    std::cout << " and packet " << ack_on->getPacketNo() << "/" << ack_on->getNumberofPackets();
//    std::cout << " for window " << ack_on->getPacketNo() / WINDOW_SIZE << std::endl;

    ack_packet->print();

    uint32_t ack_packet_size = 0;
    char * ack_flattened = ack_packet->getflatPacket(ack_packet_size);
    ErrorHandler::checkPointer(ack_flattened, "Memory allocation while flattenting ack packet failed");
    delete ack_packet;

    int len_sent = UDPSocket::_send(ack_flattened, ack_packet_size, packet_unique_id.ip, packet_unique_id.port);
    delete [] ack_flattened;
    if(len_sent == -1) {
        // Throw exception badal die
        // TODO
        ErrorHandler::die("Unable to send acknowledgment packet");

        // throw exception and catch it from the function that called us then it should propagate it as well
    }
    return len_sent;
}

bool UDPEnhancedSocket::sendWindow(const std::vector<char *>& flat_packets, const std::vector<uint32_t>& flat_packets_sizes, InternetAddress receiver_address, unsigned int start_from, unsigned int end_at_but_dont_send, uint32_t ack_timeout_usec, uint32_t resend_num) {
    unsigned int trials = 0;
    bool ack_result;
    do {
       // trials++;
        ack_result = false;
        for(unsigned int k = start_from; k < end_at_but_dont_send && !ack_result; k++) {
            int len_sent = UDPSocket::_send(flat_packets[k], flat_packets_sizes[k], receiver_address.ip, receiver_address.port);
            if(len_sent < 1) {
                // Throw exception badal die
                // TODO
                ErrorHandler::die("Unable to send packet " +  std::to_string(k) + " in sendWindow");
            }
            if(trials > 0 && k < end_at_but_dont_send - 1) {
                // Attempt to get an ack. Maybe this is the last packet the receiver dropped
                packet * this_packet = new packet(flat_packets[k]);
                ack_result = receiveAck(this_packet, ack_timeout_usec);
                delete this_packet;
            }
        }

        if(!ack_result) {
            packet * this_packet = new packet(flat_packets[end_at_but_dont_send - 1]);
            ack_result = receiveAck(this_packet, ack_timeout_usec);
            delete this_packet;
        }

        trials++;

    } while(!ack_result && trials < resend_num);

    return ack_result;
}

bool UDPEnhancedSocket::receiveAck(packet * for_packet, uint32_t ack_timeout_usec) {
    for_packet->setSenderAddress(UDPSocket::getMyAddress(), UDPSocket::getMyPort());

    char * ack_buffer = new char [BUFFER_SIZE];
    uint32_t sender_address;
    uint16_t sender_port;
    std::cout << "Attempting to receive an ack" << std::endl;
    bool ack_result = false;

    int len_received_ack = UDPSocket::_receiveWithTimeout(ack_buffer, BUFFER_SIZE, sender_address, sender_port, ack_timeout_usec);
    if(len_received_ack == -1) {
        delete [] ack_buffer;
        std::cout << "I have timed out and NOT received an ack" << std::endl;
    } else {
        packet * ack_packet = new packet(ack_buffer);

        delete [] ack_buffer;

        std::cout << "I have a received a packet. This may be the ack I'm looking for" << std::endl;

        ack_packet->print();

        std::cout << "This is the packet I'm expecting an ack for" << std::endl;

        for_packet->print();

        if(ack_packet->isAckPacket()) {
            if(ack_packet->isAckPacketFor(for_packet)) {
                ack_result = true;
                std::cout << "This is really the ack I was looking for" << std::endl;
            } else {
                std::cout << "This is an incorrect ack packet" << std::endl;
            }
        } else {
            std::cout << "This is NOT even an ack packet. I will ignore it" << std::endl;
        }

        delete ack_packet;
    }
    return ack_result;
}

bool UDPEnhancedSocket::shouldInsert(packet * p) {
    PacketKey packet_unique_id = p->getUniqueId();
    mutex_serving_map.lock();
    std::lock_guard<std::mutex> lk_serving_map(mutex_serving_map, std::adopt_lock);

    if(currently_serving.find(packet_unique_id.ip) != currently_serving.end()) {
        return false;
    } else {
        return true;
    }
}
