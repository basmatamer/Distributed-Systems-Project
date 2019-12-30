//
//  main.cpp
//  Distributed ex1
//
//  Created by MacBook Pro on 9/26/18.
//  Copyright © 2018 MacBook Pro. All rights reserved.
//

#include "DirectoryService.hpp"
#include "Server.hpp"
#include "LocalDirectory.hpp"
#include "InterfaceDefintion.h"
#include "ErrorHandler.hpp"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <csignal>
#include <thread>

DirectoryService * directory_service = NULL;
Server * server = NULL;

void directoryService_workerThreadFunction(DirectoryService * directory_service, Message* request);
void directory_service_main();

void my_handler(int s){ 
	std::cout << std::endl;   
    if(server != NULL) {
        delete server;
        server = NULL;
    }
    if(directory_service != NULL) {
        delete directory_service;
    	directory_service = NULL;
    }
    
    std::cout << "We are closed now. Come back tomorrow" << std::endl;
    exit(1);
}


int main(int argc, const char** argv) {
    std::signal(SIGINT,my_handler);
    
    directory_service_main();
    return 0;
}

void directory_service_main() {
    directory_service = new DirectoryService();
    
    server = new Server(LocalDirectory::GetPrimaryIp(), DIRECTORY_PORT);
    
    std::cout << "--> I am the directory service. Please notify others with my ip address and port number." << std::endl;
    std::cout << "--> My IP address is: " << LocalDirectory::IP_toString(LocalDirectory::GetPrimaryIp()) << std::endl;
    std::cout << "--> My port number is: " << DIRECTORY_PORT << std::endl;
    std::cout << "--> Waiting to receive requests" << std::endl;
    
    while(true) {
        Message * request = server->receiveRequest();
     	std::thread worker_thread(directoryService_workerThreadFunction, directory_service, request);
        worker_thread.detach();   
    }
}

void directoryService_workerThreadFunction(DirectoryService * directory_service, Message * request) {
	Message * reply = NULL;
	if(request == NULL) {
		std::cout << "This is a NULL request. I will ignore it" << std::endl;
		return;
	}

	std::cout << "The request is " << std::endl;
	request->print();

	switch (request->getOperation()) {
            // Requests for testing
        case OperationID::RESERVED:
            ErrorHandler::displayWarning("Using reserved operation id");
            reply = request->createEmptyReply();
            break;
        case OperationID::EMPTY:
            ErrorHandler::displayWarning("Using empty operation id");
            reply = request->createEmptyReply();
            break;
            // Requests of directory service
        case OperationID::SIGNUP:
            reply = hanle_sign_up_reqesut(directory_service, request);
            break;
        case OperationID::LOGIN:
            reply = handle_login_request(directory_service, request);
            break;
        case OperationID::HI_SERVER:
            reply = handle_announce_availability(directory_service, request);
            break;
        case OperationID::GET_ADDRESS_OF_USER:
            reply = handle_map_username_to_ip_request(directory_service, request);
            break;
        case OperationID::CHANGE_PASSWORD:
            reply = handle_change_password_request(directory_service, request);
            break;
        case OperationID::OFFER_SERVICE:
            reply = handle_offer_services_request(directory_service, request);
            break;
        case OperationID::GET_AVAILABLE_SERVICES:
            reply = handle_get_available_servies_request(directory_service, request);
            break;
        case OperationID::GET_USERNAMES:
            reply = handle_get_usernames_request(directory_service, request);
            break;
        case OperationID::SUBMIT_PENDING_QUOTA_CHANGE_REQUEST:
            reply = handle_submit_pending_quota_change_request(directory_service, request);
            break;
        case OperationID::REMOVE_PENDING_QUOTA_CHANGES_REQUEST:
            reply = handle_remove_pending_quota_changes_request(directory_service, request);
            break;
        case OperationID::TURN_OFF_DIRECTORY_SERVICE:
            ErrorHandler::displayWarning("Using TURN_OFF_DIRECTORY_SERVICE operation id.");
            ErrorHandler::displayWarning("This operation is no longer supported");
            reply = request->createEmptyReply();
            break;
        case OperationID::ANNOUNCE_IMAGE:
            reply = handle_announce_image(directory_service, request);
            break;
        default:
            ErrorHandler::displayWarning("Using unknown operation id");
            ErrorHandler::displayWarning("This unknown operation id is " + std::to_string(request->getOperation()));
            reply = request->createEmptyReply();
            break;
    }

    std::cout << "The reply is" << std::endl;
    reply->print();

    UDPEnhancedSocket * worker_socket = new UDPEnhancedSocket();
	worker_socket->sendMessage(reply, false, DEFAULT_TIMEOUT_USEC, RESEND_NO, 1);

	if(request != NULL) {
	   delete request;
	   request = NULL;
	}
	if(reply != NULL) {
	   delete reply;
	   reply = NULL;
	}
	if(worker_socket != NULL) {
	   delete worker_socket;
	   worker_socket = NULL;
	}
}
