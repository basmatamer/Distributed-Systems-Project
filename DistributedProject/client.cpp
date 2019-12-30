#include "client.hpp"
#include "ErrorHandler.hpp"
#include "LocalDirectory.hpp"
#include "API.hpp"
#include "InterfaceDefintion.h"
#include <fstream>
#include "Converter.hpp"
#include "globalvar.h"
#include <mutex>

#include "ourexception.h"

std::mutex Client::message_mutex;

Client::Client():my_username(""), my_password(""), my_server_address(LocalDirectory::getEmptyAddress()), directory_service_address(LocalDirectory::getEmptyAddress()) {
    ErrorHandler::displayWarning("Attempting to create a default Server");
    udpClientSocket = NULL;
}
Client::Client(const Client& other):my_username(other.my_username), my_password(other.my_password), my_server_address(other.my_server_address), directory_service_address(other.directory_service_address) {
    ErrorHandler::displayWarning("Attempting to copy a Client");
    udpClientSocket = new UDPClientSocket(*(other.udpClientSocket));
    ErrorHandler::checkPointer(udpClientSocket, "Memory allocation while copying Client failed");
}
Client::Client(Client&& other):my_username(other.my_username), my_password(other.my_password), my_server_address(other.my_server_address), directory_service_address(other.directory_service_address) {
    ErrorHandler::displayWarning("Attempting to move a Client");
    other.udpClientSocket = NULL;
}
Client::Client(const std::string& _my_username, const std::string& _my_password, InternetAddress _my_address, InternetAddress _my_server_address, InternetAddress _directory_service_address):my_username(_my_username), my_password(_my_password), my_server_address(_my_server_address), directory_service_address(_directory_service_address) {
    udpClientSocket = new UDPClientSocket(_my_address.ip, _my_address.port);
    ErrorHandler::checkPointer(udpClientSocket, "Memory allocation for udpClientSocket failed");
}
Client::Client(const std::string& _my_username, InternetAddress _my_address, InternetAddress _my_server_address, InternetAddress _directory_service_address):my_username(_my_username), my_server_address(_my_server_address), directory_service_address(_directory_service_address) {
    udpClientSocket = new UDPClientSocket(_my_address.ip, _my_address.port);
    ErrorHandler::checkPointer(udpClientSocket, "Memory allocation for udpClientSocket failed");
}

Client::~Client() {
    if(udpClientSocket != NULL) {
        delete udpClientSocket;
        udpClientSocket = NULL;
    }
}

uint32_t Client::assignMessageID() {
    message_mutex.lock();
    std::lock_guard<std::mutex> lk_message_id(message_mutex, std::adopt_lock);

    std::ifstream input;
    std::ofstream output;
    input.open(MESSAGES_SENT_FILENAME);
    if(input.fail()) {
        output.open(MESSAGES_SENT_FILENAME);
        output << 1;
        return 1;
    } else {
        uint32_t sent_so_far;
        input >> sent_so_far;
        input.close();
        output.open(MESSAGES_SENT_FILENAME);
        uint32_t new_id = sent_so_far + 1;
                if(new_id == 0xFFFFFFFF) {
                    new_id = 1;
                }
                output << new_id;
                return new_id;
    }
}
void Client::rollBackMessageID() {
    message_mutex.lock();
    std::lock_guard<std::mutex> lk_message_id(message_mutex, std::adopt_lock);

    std::ifstream input;
    std::ofstream output;
    input.open(MESSAGES_SENT_FILENAME);
    if(!input.fail()) {
        uint32_t sent_so_far;
        input >> sent_so_far;
        input.close();
        output.open(MESSAGES_SENT_FILENAME);
        output << sent_so_far - 1;
    }
}

InternetAddress Client::getReceiverAddress(const std::string& reciever_username) {
    //char *  args = Text_API_to_Array(sender_username);
    InternetAddress address = LocalDirectory::getEmptyAddress();

    uint32_t args_size;
    char * args = API::mapUsernameToAddress_argumentsToArray(my_password, reciever_username, args_size);

    Message * request_ip = new Message(Request, assignMessageID(), GET_ADDRESS_OF_USER, args_size, args, my_username, udpClientSocket->getMyAddress(), udpClientSocket->getMyPort(), DIRECTORY_SERVICE_USERNAME, directory_service_address.ip, directory_service_address.port);
    delete [] args;

    Message * reply_ip;

    try{
        reply_ip = _send(request_ip, DEFAULT_TIMEOUT_USEC);
    } catch(const SendingFailedException& except) {
        delete request_ip;
        rollBackMessageID();
        std::cout << "Client::getReceiverAddress " << except.what() << std::endl;
        throw except;
    }

    delete request_ip;

    if(reply_ip == NULL) {
        rollBackMessageID();
        throw DirectoryServiceUnreachableException("Directory Service is unreachable");

    } else {
        uint32_t results_size = reply_ip->getArgumentsSize();
        char * results = new char [results_size];
        reply_ip->getArguments(results, results_size);
        bool is_mapping_successful;
        API::mapUsernameToaddress_arrayToResults(results, is_mapping_successful, address);

        if(!is_mapping_successful) {
            throw UnknownUsernameException("Unknown username " + reciever_username);
        }

        delete reply_ip;
    }

    return address;
}

Message * Client::_send(Message * _message, unsigned int timeout_usec) {

    ErrorHandler::checkPointer(udpClientSocket, "Attempting to execute a client message while the UDP Client Socket is NULL. Fatal Error.");

    // std::cout << "The message id in Client::execute is " << message_id << std::endl;
    // std::cout << "The message id in the _message in Client::execute is " <<  _message->getMessageId() << std::endl;

    // std::cout << "The message size in Client::execute is " << _message->getTotalSize() << std::endl;


    Message * reply;

    try {
        reply = udpClientSocket->sendWithGuarantee(_message, timeout_usec);

    } catch(const SendingFailedException& except) {
        std::cout << "Client::_send " << except.what() << std::endl;
        throw except;
    }

    if(reply == NULL) {
        ErrorHandler::displayWarning("No reply was received");
    }

    return reply;
}

// If reply is NULL, then the receiver was unreachable. Display a message to the user.
// If reply is not NULL, then handle reply accordingly.
Message * Client::sendMessage(Message * _message, unsigned int timeout_usec) {
    ErrorHandler::checkPointer(udpClientSocket, "Attempting to execute a client message while the UDP Client Socket is NULL. Fatal Error.");

    // Get the address of the receiver
    InternetAddress receiver_address;
    std::string receiver_username = _message->getReceiverUsername();
    receiver_address.ip = _message->getReceiverIP();
    receiver_address.port = _message->getReceiverIP();

    MessageTargetType target_type = getTargetType(OperationID(_message->getOperation()));

    try {
        switch (target_type) {
            case UNKOWN:
                break;
            case MYSELF:
                receiver_address = my_server_address;
                receiver_username = my_username;
                break;
            case ANOTHER_PEER:
                receiver_address = getReceiverAddress(_message->getReceiverUsername());
                break;
            case DIRECTORY_SERVICE:
                receiver_address = directory_service_address;
                receiver_username = DIRECTORY_SERVICE_USERNAME;
                break;
        }
    } catch(const DirectoryServiceUnreachableException& except) {
        std::cout << "Client::sendMessage " << except.what() << std::endl;
        throw except;
    } catch(const UnknownOperationException& except) {
        std::cout << "Client::sendMessage " << except.what() << std::endl;
        throw except;
    }  catch(const SendingFailedException& except) {
        std::cout << "Client::sendMessage " << except.what() << std::endl;
        throw except;
    }

    _message->setReceiver(receiver_username, receiver_address.ip, receiver_address.port);
    _message->setSender(my_username, udpClientSocket->getMyAddress(), udpClientSocket->getMyPort());

    // Assign a message ID
    if(_message->getOperation() != OperationID::SIGNUP) {
        _message->setMessageId(assignMessageID());
    } else {
        _message->setMessageId(0);
    }
   // std::cout << "The message id in Client::execute is " << message_id << std::endl;
   // std::cout << "The message id in the _message in Client::execute is " <<  _message->getMessageId() << std::endl;
    _message->print();
   // std::cout << "The message size in Client::execute is " << _message->getTotalSize() << std::endl;
    Message * reply;

    try {
        reply = _send(_message, timeout_usec);
    } catch(const SendingFailedException& except) {
        std::cout << "Client::sendMessage " << except.what() << std::endl;
        throw except;
    }

    if(reply == NULL) {
        ErrorHandler::displayWarning("No reply was received");
        rollBackMessageID();

        if(_message->getOperation() == UPDATE_VIEWS_COUNTER) {
            uint32_t new_args_size = _message->getArgumentsSize() + 4 + (uint32_t)(my_password.length());
            char * news_args = new char[new_args_size];

            ErrorHandler::checkPointer(news_args, "Memory allocation while creating arguments for creating a pending change message failed");

            char * pointer = news_args;
            Converter::int_to_chars((uint32_t)(my_password.length()), pointer);
            pointer += 4;
            Converter::string_to_chars(my_password, pointer);
            pointer += (uint32_t)(my_password.length());

            _message->getArguments(pointer, _message->getArgumentsSize());

            Message * new_message = new Message(Request, assignMessageID(), SUBMIT_PENDING_QUOTA_CHANGE_REQUEST, new_args_size, news_args, my_username, udpClientSocket->getMyAddress(), udpClientSocket->getMyAddress(), DIRECTORY_SERVICE_USERNAME, directory_service_address.ip, directory_service_address.port);
            delete [] news_args;


            Message * new_reply = _send(new_message, timeout_usec);
            delete new_message;

            if(new_reply == NULL) {
                throw DirectoryServiceUnreachableException("Unable to reach the directory service");
            }

            return new_reply;
        }

        if(target_type == DIRECTORY_SERVICE) {
            throw DirectoryServiceUnreachableException("Unable to reach the directory service");
        }

    }

    return reply;
}

void Client::setMyPassword(const std::string& _password) {
    my_password = _password;
}
std::string Client::getMyPassword() const {
    return my_password;
}
