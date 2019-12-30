
#ifndef UDPClientSocket_hpp
#define UDPClientSocket_hpp

#include "UDPEnhancedSocket.hpp"
#include "Message.hpp"

#include "InterfaceDefintion.h"

class UDPClientSocket: public UDPEnhancedSocket {

public:
    UDPClientSocket();
    UDPClientSocket(uint32_t _myAddr, uint16_t _myPort);
    ~UDPClientSocket();

    void sendNoGuarantee(Message * message);
    Message * sendWithGuarantee(Message * message, unsigned int timeout_usec, uint32_t resend_num = RESEND_NO);

};

#endif /* UDPClientSocket_hpp */
