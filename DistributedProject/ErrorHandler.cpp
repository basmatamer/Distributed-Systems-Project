#include "ErrorHandler.hpp"
#include <iostream>
#include <stdlib.h>
void ErrorHandler::die(const std::string& _message) {
    std::cerr <<_message << std::endl;
    exit(1);
}

void ErrorHandler::displayWarning(const std::string& _message) {
    std::string warning_message = "Warning: " + _message;
    std::cout << warning_message << std::endl;
}

void ErrorHandler::checkPointer(void* _pointer, const std::string& _error_message) {
    if(_pointer == NULL) {
        die(_error_message);
    }
}
