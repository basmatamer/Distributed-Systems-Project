#include <vector>
#include <cmath>

#include "UDPClientSocket.hpp"

#include "LocalDirectory.hpp"
#include "ErrorHandler.hpp"
#include "packet.hpp"
#include "Message.hpp"
#include "InterfaceDefintion.h"

#include "ourexception.h"

UDPClientSocket::UDPClientSocket():UDPEnhancedSocket() {
}
//UDPClientSocket::UDPClientSocket(uint32_t _myAddr, uint16_t _myPort):UDPEnhancedSocket(_myAddr, _myPort) {
//}
// Now a client socket can decide its port number on its own
UDPClientSocket::UDPClientSocket(uint32_t _myAddr, uint16_t _myPort):UDPEnhancedSocket() {
}

UDPClientSocket::~UDPClientSocket() {
}

void UDPClientSocket::sendNoGuarantee(Message * message) {
    try {
        sendMessage(message, false, 1, 0, 1);
    } catch(const SendingFailedException& except) {
        std::cout << "UDPClientSocket::sendNoGuarantee:: " << except.what() << std::endl;
    }
}

Message * UDPClientSocket::sendWithGuarantee(Message * message, unsigned int timeout_usec, uint32_t resend_num) {
    uint32_t timeout_ack_usec = DEFAULT_TIMEOUT_ACK_USEC;
    if(getTargetType(OperationID(message->getOperation())) == MessageTargetType::DIRECTORY_SERVICE) {
        timeout_ack_usec = DEFAULT_TIMEOUT_ACK_DIRECTORY_USEC;
    }

    Message * reply_message;

    try {
        reply_message = sendMessage(message, true, timeout_ack_usec, resend_num, timeout_usec);

    } catch(const SendingFailedException& except) {
        if(packetsMap.size() > 0) {
            ErrorHandler::displayWarning("Packets map still contains some entries from previous send");
            ErrorHandler::displayWarning( "Dumping map data ..");
            for (std::map<PacketKey, std::vector<packet> >::iterator it = packetsMap.begin(); it != packetsMap.end(); it++) {
                PacketKey key = it->first;
                std::cerr << LocalDirectory::IP_toString(key.ip) << " at " << key.port << " with id " << key.message_id << " still contains " << it->second.size() << " packets" << std::endl;
            }
            packetsMap.clear();

        }

        std::cout << "UDPClientSocket::sendWithGuarantee " << except.what() << std::endl;
        throw except;
    }

    if(packetsMap.size() > 0) {
        ErrorHandler::displayWarning("Packets map still contains some entries from previous send");
        ErrorHandler::displayWarning( "Dumping map data ..");
        for (std::map<PacketKey, std::vector<packet> >::iterator it = packetsMap.begin(); it != packetsMap.end(); it++) {
            PacketKey key = it->first;
            std::cerr << LocalDirectory::IP_toString(key.ip) << " at " << key.port << " with id " << key.message_id << " still contains " << it->second.size() << " packets" << std::endl;
        }
        packetsMap.clear();

    }
    return reply_message;
}
