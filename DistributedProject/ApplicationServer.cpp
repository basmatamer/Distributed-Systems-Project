#include "ApplicationServer.h"
#include "Message.hpp"
#include "InterfaceDefintion.h"
#include "Server.hpp"
#include "LocalDirectory.hpp"
#include "API.hpp"
#include "globalvar.h"
#include "image.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>

#include "ourexception.h"

#include "Configs.h"

void ApplicationServer::serverMain(uint32_t ip_address, uint16_t port_number) {
    std::cout << "Creating main server" << std::endl;

    Server* main_server;
    try {
        main_server = new Server(ip_address, port_number);
    } catch(const SocketBindingFailedException& except) {
        std::cout << except.what() << std::endl;
        return;
    } catch(const SocketCreationFailedException& except) {
        std::cout << except.what() << std::endl;
        return;
    }

    bool should_exit = false;

    try {
        if(main_server == NULL) {
            std::cerr << "The main server is NULL" << std::endl;
            return;
        } else {
            do {
                Message * new_message;
                new_message = main_server->receiveRequest();
                if(new_message== NULL) {
                    std::cerr << "Message received is NULL" << std::endl;
                    continue;
                } else {
                    should_exit = (new_message->getOperation() == LOGOUT) && (new_message->getSenderUsername() == new_message->getReceiverUsername());
                    if(should_exit) {
                        std::cout << "We should exit right now" << std::endl;
                        handle_logout(new_message);
                    } else {
                        std::thread worker_thread(ApplicationServer::handle_anyRequest, new_message);
                        worker_thread.detach();
                    }
                }

            } while(!should_exit);
        }

    } catch(const SendingFailedException& except) {
        std::cout << "ApplicationServer::serverMain " << except.what() << std::endl;
    }

    std::cout << "Deleting main server" << std::endl;
    delete main_server;
    std::cout << "Main server deleted" << std::endl;
}


void ApplicationServer::handle_anyRequest(Message * new_message) {
    std::cout << "This message just came in: " << std::endl;
    std::cout << new_message->getSenderUsername() << " at " << LocalDirectory::IP_toString(new_message->getSenderIP());
    std::cout << " (" << new_message->getSenderPort() << "): ";

    switch(new_message->getOperation()) {
        case TEXT_MESSAGE:
            handle_textMessage(new_message);
            break;
        case REQUEST_IMAGE:
            handle_requestImage(new_message);
            break;
        case REQUEST_ADDITIONAL_VIEWS:
            handle_requestViews(new_message);
            break;
        case SEND_IMAGE:
            handle_sendImage(new_message);
            break;
        case UPDATE_VIEWS_COUNTER:
            handle_changeQuota(new_message);
            break;
        case RETREIVE_STATISTICS:
            roam_service_server(new_message);
            break;

        default:
            std::cout << "An unknwon message has arrived. I'm not a superhero server." << std::endl;
            break;
    }
}

void ApplicationServer::handle_logout(Message* request) {
    // Decoding the message

    // Processing the request

    // Sending the reply
    Message * reply = request->createEmptyReply();
    delete request;

    UDPEnhancedSocket* worker_socket;
    try {
        worker_socket = new UDPEnhancedSocket();
    } catch(const SocketBindingFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    } catch(const SocketCreationFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    }

    try {
        worker_socket->sendMessage(reply, false, DEFAULT_TIMEOUT_USEC, RESEND_NO, 1);
    } catch(const SendingFailedException& except) {
        std::cout << "ApplicationServer::logout " << except.what() << std::endl;
    }
    delete reply;
    delete worker_socket;
}


void ApplicationServer::handle_textMessage(Message * request) {
    // Decoding the message
    uint32_t args_size = request->getArgumentsSize();
    char * args = new char [args_size];
    request->getArguments(args, args_size);

    std::string text_msg;
    API::text_arrayToArguments(args, text_msg);
    delete [] args;

   //  std::cout << text_msg << std::endl;

    // Processing the request
    std::string reply_msg = "Thank you for sending this message. " + text_msg;

    // Sending the reply
    uint32_t reply_size;
    char * reply_args = API::text_resultsToArray(reply_msg, reply_size);

    Message * reply = request->createReply(reply_size, reply_args);
    delete [] reply_args;
    delete request;

    UDPEnhancedSocket* worker_socket;
    try {
        worker_socket = new UDPEnhancedSocket();
    } catch(const SocketBindingFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    } catch(const SocketCreationFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    }

    try {
        worker_socket->sendMessage(reply, false, DEFAULT_TIMEOUT_USEC, RESEND_NO, 1);
    } catch(const SendingFailedException& except) {
        std::cout << "ApplicationServer::text " << except.what() << std::endl;
    }
    delete reply;
    delete worker_socket;
}

void ApplicationServer::handle_requestImage(Message * request) {
    // Decoding the message
    uint32_t args_size = request->getArgumentsSize();
    char * args = new char [args_size];
    request->getArguments(args, args_size);

    // std::cout << "The arguments size is " << args_size << std::endl;
    ImageReference img_ref;
    uint32_t views_no;
    API::requestImage_arrayToArguments(args, img_ref, views_no);
    delete [] args;

    std::string sender_username = request->getSenderUsername();

    RemoteImageReference RIR;
    RIR.img_ref = img_ref;
    RIR.shared_with_username = sender_username;

//    std::cout << "The sender is " << sender_username << std::endl;
//    std::cout << "The shared_with_username is " << RIR.shared_with_username << std::endl;
//    std::cout << "The image is " << RIR.img_ref.image_name << std::endl;
//    std::cout << "The owner username is " << RIR.img_ref.owner_username << std::endl;

    // Processing the request

    any_log->write_pending_requests(RIR, views_no);

    // Sending the reply
    uint32_t reply_size;
    char* reply_args;

    reply_args = API::replyWithRIRAck_argumentsToArray(RIR, views_no, reply_size);

    Message * reply = request->createReply(reply_size, reply_args);
    delete [] reply_args;
    delete request;

    UDPEnhancedSocket* worker_socket;
    try {
        worker_socket = new UDPEnhancedSocket();
    } catch(const SocketBindingFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    } catch(const SocketCreationFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    }

    try {
        worker_socket->sendMessage(reply, false, DEFAULT_TIMEOUT_USEC, RESEND_NO, 1);
    } catch(const SendingFailedException& except) {
        std::cout << "ApplicationServer::requestImage " << except.what() << std::endl;
    }
    delete reply;
    delete worker_socket;
}

 void ApplicationServer::handle_requestViews(Message * request) {
     // Decoding the message
     uint32_t args_size = request->getArgumentsSize();
     char * args = new char [args_size];
     request->getArguments(args, args_size);

     ImageReference img_ref;
     uint32_t views_no;
     API::requestviews_arrayToArguments(args, img_ref, views_no);
     delete [] args;

     std::string sender_username = request->getSenderUsername();

     RemoteImageReference RIR;
     RIR.img_ref = img_ref;
     RIR.shared_with_username = sender_username;

     std::cout << "The sender is " << sender_username << std::endl;
     std::cout << "The shared_with_username is " << RIR.shared_with_username << std::endl;
     std::cout << "The image is " << RIR.img_ref.image_name << std::endl;
     std::cout << "The owner username is " << RIR.img_ref.owner_username << std::endl;

     // Processing the request
     any_log->write_pending_requests(RIR, views_no);

     // Sending the reply
     Message * reply = request->createEmptyReply();
     delete request;

     UDPEnhancedSocket* worker_socket;
     try {
         worker_socket = new UDPEnhancedSocket();
     } catch(const SocketBindingFailedException& except) {
         delete reply;
         std::cout << except.what() << std::endl;
         return;
     } catch(const SocketCreationFailedException& except) {
         delete reply;
         std::cout << except.what() << std::endl;
         return;
     }

     worker_socket->sendMessage(reply, false, DEFAULT_TIMEOUT_USEC, RESEND_NO, 1);
     try {
         worker_socket->sendMessage(reply, false, DEFAULT_TIMEOUT_USEC, RESEND_NO, 1);
     } catch(const SendingFailedException& except) {
         std::cout << "ApplicationServer::requestViews " << except.what() << std::endl;
     }
     delete reply;
     delete worker_socket;
 }

void ApplicationServer::handle_changeQuota(Message * request) {
    // Decoding the message
    uint32_t args_size = request->getArgumentsSize();
    char * args = new char [args_size];
    request->getArguments(args, args_size);

    PendingQuotaChange quota_change;

    API::updateViews_arrayToArguments(args, quota_change);
    delete [] args;

    std::string sender_username = request->getSenderUsername();

    // Processing the request
    std::thread worker_thread(ApplicationServer::update_image, sender_username, quota_change.remote_img_ref.img_ref.image_name, quota_change.new_quota);

    // Sending the reply
    Message * reply = request->createEmptyReply();
    delete request;

    UDPEnhancedSocket* worker_socket;
    try {
        worker_socket = new UDPEnhancedSocket();
    } catch(const SocketBindingFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    } catch(const SocketCreationFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    }

    try {
        worker_socket->sendMessage(reply, false, DEFAULT_TIMEOUT_USEC, RESEND_NO, 1);
    } catch(const SendingFailedException& except) {
        std::cout << "ApplicationServer::changeQuota " << except.what() << std::endl;
    }

    delete reply;
    delete worker_socket;

    worker_thread.join();
}
void ApplicationServer::update_image(std::string owner_username, std::string image_name, uint32_t new_quota) {
    image::updateQuota(shared_images_path+owner_username+"/", "local_" + user_username_string+ "_"+ image_name,  new_quota);

//    try {
//        ImageReferenceMap img_ref_map;
//        img_ref_map.image_name = image_name;
//        img_ref_map.owner_username = owner_username;
//        locks_on_images.at(img_ref_map)->lock();
//        image::updateQuota(shared_images_path+owner_username+"/", "local_" + user_username_string+ "_"+ image_name,  new_quota);
//        locks_on_images.at(img_ref_map)->unlock();


//    } catch (...) {
//        image::updateQuota(shared_images_path+owner_username+"/", "local_" + user_username_string+ "_"+ image_name,  new_quota);
//        std::cout << "Warning .. Unable to find the mutex" << std::endl;
//    }
}

void ApplicationServer::handle_sendImage(Message * request) {
    // Decoding the message
    uint32_t reply_size = request->getArgumentsSize();
    char * buffer = new char[reply_size];
    request->getArguments(buffer, reply_size);
    bool result;
    std::string image_string;
    RemoteImageReference RIR;
    API::replyWithImage_arrayToArguments(buffer, image_string, RIR, result);
    delete [] buffer;

    std::cout << "The request was " << (result ? "accepted" : "rejected") << std::endl;
    std::cout << "The RIR is" << std::endl;
    std::cout << "owner: " << RIR.img_ref.owner_username << std::endl;
    std::cout << "image name: " << RIR.img_ref.image_name << std::endl;
    std::cout << "Shared with name: " << RIR.shared_with_username << std::endl;

    std::cout << "The size of the image string is " << image_string.length() << std::endl;

    // Processing the request
    // TODO
    // Either thread this or send it the reply first
    std::thread worker_thread(ApplicationServer::save_image, result, RIR.img_ref.owner_username, RIR.img_ref.image_name, &image_string);

    // Sending the reply
    Message * reply = request->createEmptyReply();
    std::cout << "The reply is " << std::endl;
    reply->print();
    delete request;

    UDPEnhancedSocket* worker_socket;
    try {
        worker_socket = new UDPEnhancedSocket();
    } catch(const SocketBindingFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    } catch(const SocketCreationFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    }

    try {
        worker_socket->sendMessage(reply, false, DEFAULT_TIMEOUT_USEC, RESEND_NO, 1);
    } catch(const SendingFailedException& except) {
        std::cout << "ApplicationServer::sendImage " << except.what() << std::endl;
    }

    delete reply;
    delete worker_socket;

    worker_thread.join();
}
void ApplicationServer::save_image(bool result, std::string owner_username, std::string image_name, std::string* image_string) {
    if(result) {
        std::cout << "Attempting to save the image that we received" << std::endl;
        std::string folder_path = shared_images_path + owner_username + "/";
        std::cout << "Checking the path: " << folder_path << std::endl;
        if(!checkDirectoryExists(folder_path)) {
            std::cout << "Creating a folder" << std::endl;
            createFolder(folder_path);
        }
        image::textToImg(folder_path, "local_" + user_username_string + "_" + image_name, *image_string);

//        ImageReferenceMap img_ref_map;
//        img_ref_map.image_name = image_name;
//        img_ref_map.owner_username = owner_username;
//        locks_on_images[img_ref_map] = new std::mutex();

        std::cout << "Image (inshAllah) saved" << std::endl;
    }

}

void ApplicationServer::roam_service_server(Message* request)
{
    std::vector<Statistic> my_views;    // my views as a server of all images that this sender has shared with me !!

    std::string sender_username;
    uint32_t _ip;
    uint16_t _port;

    ifstream myfile;


    request->getSender(sender_username, _ip, _port);   // returns the sender_username!!

    std:: vector<std::string> files;
    files = GetDirectoryFiles(shared_images_path + sender_username);

    for(unsigned int i = 0; i < files.size();  i++)
    {
        int actual_views = 0;
        if(files[i].find(".jpg") != files[i].length() - 4) {
            continue;
        }

        actual_views = image::getViewsForRobot(shared_images_path + sender_username + "/", files[i]);


//        try {
//            ImageReferenceMap img_ref_map;
//            img_ref_map.image_name = files[i].substr(files[i].find_last_of('_'), files[i].length());
//            img_ref_map.owner_username = sender_username;
//            locks_on_images.at(img_ref_map)->lock();
//            actual_views = image::getViewsForRobot(shared_images_path + sender_username + "/", files[i]);
//            locks_on_images.at(img_ref_map)->unlock();


//        } catch (...) {
//            actual_views = image::getViewsForRobot(shared_images_path + sender_username + "/", files[i]);
//            std::cout << "Warning .. Unable to find the mutex" << std::endl;
//        }

       // if(actual_views == 0)


        Statistic image_statistic;
        image_statistic.image_name= files[i];   // will remove from it anything coming after . (ya3ne remove the extension)
        image_statistic.views_no = actual_views;
        image_statistic.shared_with_username = user_username_string;
        my_views.push_back(image_statistic);

    }

    Message * reply;
    char* reply_args;
    uint32_t reply_size;

    // need to marshal this vector called my_views!!!!
    reply_args = API::retrievedStatistics_argumentsToArray(my_views, reply_size);

    // constructing the reply
    //Message* reply = new Message(Reply, 0, RETREIVE_STATISTICS, args_size, args,  "", 0, 0, sender_username, 0, 0);

    std::cout << "Attempt to send retrieved statstics reply" << std::endl;
    reply = request->createReply(reply_size, reply_args);

    std::cout << "Retrieved statstics reply sent" << std::endl;
    delete [] reply_args;
    delete request;

    UDPEnhancedSocket* worker_socket;
    try {
        worker_socket = new UDPEnhancedSocket();
    } catch(const SocketBindingFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    } catch(const SocketCreationFailedException& except) {
        delete reply;
        std::cout << except.what() << std::endl;
        return;
    }

    try {
        worker_socket->sendMessage(reply, false, DEFAULT_TIMEOUT_USEC, RESEND_NO, 1);
    } catch(const SendingFailedException& except) {
        std::cout << "ApplicationServer::statistics " << except.what() << std::endl;
    }

    delete reply;
    delete worker_socket;
}










