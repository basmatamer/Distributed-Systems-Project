//
//  UDPServerSocket.cpp
//  Distributed Project
//
//  Created by Mohamed on 10/27/18.
//  Copyright © 2018 Mohamed Tawfik. All rights reserved.
//

#include "UDPServerSocket.hpp"
#include "ErrorHandler.hpp"
#include "InterfaceDefintion.h"
#include <vector>
#include <mutex>

#include "ourexception.h"

UDPServerSocket::UDPServerSocket():UDPEnhancedSocket() {

}
UDPServerSocket::UDPServerSocket(uint32_t _myAddr, uint16_t _myPort):UDPEnhancedSocket(_myAddr, _myPort) {

}
UDPServerSocket::~UDPServerSocket() {

}

Message * UDPServerSocket::receive(uint32_t buffer_size) {
    Message * new_message;
    try {
        new_message = receiveMessage(buffer_size, 0);
    } catch(const SendingFailedException& except) {
        std::cout << except.what() << std::endl;
        throw except;
    }

    mutex_serving_map.lock();
    std::lock_guard<std::mutex> lk_serving_map(mutex_serving_map, std::adopt_lock);

    currently_serving[new_message->getSenderIP()] = new_message->getSenderIP();

    return new_message;
}

void UDPServerSocket::reply(Message * message) {
    try {
        sendMessage(message, false, DEFAULT_TIMEOUT_USEC, RESEND_NO, 1);
    } catch(const SendingFailedException& except) {
        std::cout << "UDPServerSocket::reply " << except.what() << std::endl;
        throw except;
    }
}
