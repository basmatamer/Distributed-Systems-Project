//
//  UDPServerSocket.hpp
//  Distributed Project
//
//  Created by Mohamed on 10/27/18.
//  Copyright © 2018 Mohamed Tawfik. All rights reserved.
//

#ifndef UDPServerSocket_hpp
#define UDPServerSocket_hpp

#include "UDPEnhancedSocket.hpp"
#include "packet.hpp"
#include "Message.hpp"
#include <vector>
#include <map>

//enum PacketInsertResult{duplicate = 0, success = 1, message_completed = 2};

class UDPServerSocket: public UDPEnhancedSocket {

public:
    UDPServerSocket();
    UDPServerSocket(uint32_t _myAddr, uint16_t _myPort);
    ~UDPServerSocket();

    Message * receive(uint32_t buffer_size);
    void reply(Message * message);

};

#endif /* UDPServerSocket_hpp */
