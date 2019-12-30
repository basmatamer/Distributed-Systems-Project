//
//  ErrorHandler.hpp
//  Distributed ex1
//
//  Created by MacBook Pro on 9/26/18.
//  Copyright © 2018 MacBook Pro. All rights reserved.
//

#ifndef ErrorHandler_hpp
#define ErrorHandler_hpp

#include <string>

class ErrorHandler {
public:
    static void die(const std::string& _message);
    static void displayWarning(const std::string& _message);
    static void checkPointer(void* _pointer, const std::string& _error_message);
};

#endif /* ErrorHandler_hpp */
