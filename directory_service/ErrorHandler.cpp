//
//  ErrorHandler.cpp
//  Distributed ex1
//
//  Created by MacBook Pro on 9/26/18.
//  Copyright © 2018 MacBook Pro. All rights reserved.
//

#include "ErrorHandler.hpp"
#include <iostream>
#include <stdlib.h>
void ErrorHandler::die(const std::string& _message) {
    std::cerr <<_message << std::endl;
    exit(1);
}

void ErrorHandler::displayWarning(const std::string& _message) {
    std::string warning_message = "Warning: " + _message;
    std::cerr << warning_message << std::endl;
}

void ErrorHandler::checkPointer(void* _pointer, const std::string& _error_message) {
    if(_pointer == NULL) {
        die(_error_message);
    }
}
