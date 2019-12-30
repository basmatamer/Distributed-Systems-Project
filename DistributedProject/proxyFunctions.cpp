#include "Message.hpp"
#include "ErrorHandler.hpp"
#include "Converter.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "proxyFunctions.hpp"
#include "InterfaceDefintion.h"
#include "image.h"
#include <vector>
#include "API.hpp"
#include "Configs.h"

#include <chrono>
#include <thread>

#include <ourexception.h>

#define timeout DEFAULT_TIMEOUT_USEC

bool proxyFunctions::signup(Client * myClient, std::string username, std::string password, std::string& display_msg) {
    unsigned int args_size;
    char * args  = API::signup_argumentsToArray(password, args_size);

    Message * signup_info = new Message(MessageType::Request, 0, OperationID::SIGNUP, args_size, args,
    username, 0, 0, DIRECTORY_SERVICE_USERNAME, 0, 0);
    delete [] args;

    Message * signupReply;

    try {
        signupReply = myClient->sendMessage(signup_info, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(signup_info != NULL) {
            delete signup_info;
            signup_info = NULL;
        }
        throw except;
    } catch(const UnknownUsernameException& except) {
        if(signup_info != NULL) {
            delete signup_info;
            signup_info = NULL;
        }
        throw except;
    } catch(const SendingFailedException& except) {
        if(signup_info != NULL) {
            delete signup_info;
            signup_info = NULL;
        }
        throw except;
    }


    if(signup_info != NULL) {
        delete signup_info;
        signup_info = NULL;
    }

    if(signupReply == NULL) {
        ErrorHandler::displayWarning("Failed to signup: ERROR NULL");
        return false;
    } else {
        std::cout << "The signup reply is " << std::endl;

        signupReply->print();

        uint32_t reply_size = signupReply->getArgumentsSize();
        if(reply_size == 0) {
            ErrorHandler::displayWarning("Failed to signup: Reply arguments size is 0");
            delete signupReply;
            return false;
        }

        char * buffer = new char[reply_size];
        signupReply->getArguments(buffer, reply_size);
        delete signupReply;

        bool isAccepted = false;
        API::signup_arrayToResults(buffer, isAccepted, display_msg);
        delete [] buffer;

        if(isAccepted) {
            myClient->setMyPassword(password);
        }

        return isAccepted;
    }
}
bool proxyFunctions::login(Client * myClient, std::string username, std::string password) {
    uint32_t args_size;
    char * args  = API::login_argumentsToArray(password, args_size);

    Message * login_info = new Message(MessageType::Request, 0, OperationID::LOGIN, args_size, args,
    username, 0, 0, DIRECTORY_SERVICE_USERNAME, 0, 0);
    delete [] args;

    Message * login_reply;

    try {
        login_reply = myClient->sendMessage(login_info, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(login_info != NULL) {
            delete login_info;
            login_info = NULL;
        }
        throw except;
    } catch(const UnknownUsernameException& except) {
        if(login_info != NULL) {
            delete login_info;
            login_info = NULL;
        }
        throw except;
    } catch(const SendingFailedException& except) {
        if(login_info != NULL) {
            delete login_info;
            login_info = NULL;
        }
        throw except;
    }


    if(login_info != NULL) {
        delete login_info;
        login_info = NULL;
    }


    if(login_reply == NULL) {
        ErrorHandler::displayWarning("Failed to login: ERROR NULL");
        return false;
    }
	
    uint32_t reply_size = login_reply->getArgumentsSize();
    if(reply_size == 0) {
        ErrorHandler::displayWarning("Failed to login: Reply arguments size is 0");
        delete login_reply;
        return false;
    } else {
        char * buffer = new char[reply_size];
        login_reply->getArguments(buffer, reply_size);
        delete login_reply;

        bool isAccepted = false;
        API::login_arrayToResults(buffer, isAccepted);
        delete [] buffer;

        if(isAccepted) {
            myClient->setMyPassword(password);
        }

        return isAccepted;
    }
}
void proxyFunctions::hiServer(Client * myClient, InternetAddress address) {

    uint32_t args_size;
    char * args  = API::hiServer_argumentsToArray(myClient->getMyPassword(), address.ip, address.port, args_size);

    Message * hiServer_info = new Message(MessageType::Request, 0, OperationID::HI_SERVER, args_size, args,
    "", 0, 0, DIRECTORY_SERVICE_USERNAME, 0, 0);
    delete [] args;

    Message * hiServer_reply;
    try {
       hiServer_reply = myClient->sendMessage(hiServer_info, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(hiServer_info != NULL) {
            delete hiServer_info;
            hiServer_info = NULL;
        }
        throw except;
    } catch(const UnknownUsernameException& except) {
        if(hiServer_info != NULL) {
            delete hiServer_info;
            hiServer_info = NULL;
        }
        throw except;
    } catch(const SendingFailedException& except) {
        if(hiServer_info != NULL) {
            delete hiServer_info;
            hiServer_info = NULL;
        }
        throw except;
    }

    if(hiServer_info != NULL) {
        delete hiServer_info;
        hiServer_info = NULL;
    }
	
    if(hiServer_reply ==  NULL) {
		ErrorHandler::displayWarning("Failed to welcome server: ERROR NULL");
        return;
    } else {
        uint32_t reply_size = hiServer_reply->getArgumentsSize();
        if(reply_size == 0) {
            ErrorHandler::displayWarning("Failed to welcome server: Reply arguments size is 0");
            delete hiServer_reply;
            return;
        }

        char * buffer = new char[reply_size];
        hiServer_reply->getArguments(buffer, reply_size);
        delete hiServer_reply;

        std::vector<PendingQuotaChange> changes;
        API::hiServer_arrayToResults(buffer, changes);
        delete [] buffer;

        std::cout << "There are " << changes.size() << " changes to commit" << std::endl;



        if(changes.size() > 0) {
            if(proxyFunctions::removePending(myClient)) {
                for(unsigned int i = 0; i < changes.size(); i++) {
                    std::cout << shared_images_path + changes[i].remote_img_ref.img_ref.owner_username << "/";
                    std::cout << "local_" + user_username_string + "_" + changes[i].remote_img_ref.img_ref.image_name << " ";
                    std::cout << changes[i].new_quota << std::endl;

                    image::updateQuota(shared_images_path + changes[i].remote_img_ref.img_ref.owner_username + "/",
                                       "local_" + user_username_string + "_" + changes[i].remote_img_ref.img_ref.image_name,
                                       changes[i].new_quota);

//                    try {
//                        ImageReferenceMap img_ref_map;
//                        img_ref_map.image_name = changes[i].remote_img_ref.img_ref.image_name;
//                        img_ref_map.owner_username = changes[i].remote_img_ref.img_ref.owner_username;
//                        locks_on_images.at(img_ref_map)->lock();
//                        image::updateQuota(shared_images_path + changes[i].remote_img_ref.img_ref.owner_username + "/",
//                                           "local_" + user_username_string + "_" + changes[i].remote_img_ref.img_ref.image_name,
//                                           changes[i].new_quota);
//                        locks_on_images.at(img_ref_map)->unlock();

//                    } catch(...) {
//                        image::updateQuota(shared_images_path + changes[i].remote_img_ref.img_ref.owner_username + "/",
//                                           "local_" + user_username_string + "_" + changes[i].remote_img_ref.img_ref.image_name,
//                                           changes[i].new_quota);
//                    }


                }


           }
        }
    }
}
bool proxyFunctions::removePending(Client * myClient) {

    uint32_t args_size;
    char * args = API::removePendingQuotaChanges_argumentsToArray(myClient->getMyPassword(), args_size);

    Message * remove_pending = new Message(MessageType::Request, 0, OperationID::REMOVE_PENDING_QUOTA_CHANGES_REQUEST, args_size, args,
    "", 0, 0, DIRECTORY_SERVICE_USERNAME, 0, 0);
    delete [] args;

    Message * remove_pending_reply;
    try {
        std::cout << "Attempting to remove pending" << std::endl;

       remove_pending_reply = myClient->sendMessage(remove_pending, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        std::cout << "Exception in directory service remove pending" << std::endl;

        if(remove_pending != NULL) {
            delete remove_pending;
            remove_pending = NULL;
            return false;
        }
    } catch(const UnknownUsernameException& except) {
        std::cout << "Exception in unknown username remove pending" << std::endl;

        if(remove_pending != NULL) {
            delete remove_pending;
            remove_pending = NULL;
            return false;
        }
    } catch(const SendingFailedException& except) {
        std::cout << "Exception in sending failed remove pending" << std::endl;

        if(remove_pending != NULL) {
            delete remove_pending;
            remove_pending = NULL;
            return false;
        }
    }

    std::cout << "No exception in remove pending" << std::endl;


    if(remove_pending != NULL) {
        delete remove_pending;
        remove_pending = NULL;
    }

    if(remove_pending_reply ==  NULL) {
        ErrorHandler::displayWarning("Failed to tell directory of submitted changes: ERROR NULL");
        return false;
    } else {
        std::cout << "The reply was succesful in remove pending changes" << std::endl;
        delete remove_pending_reply;
        return true;
    }
}

bool proxyFunctions::changePassword(Client* myClient, std::string old_password, std::string new_password) {
    uint32_t args_size;
    char * args  = API::changePassword_argumentsToArray(old_password, new_password, args_size);

    Message * change_password_msg = new Message(MessageType::Request, 0, OperationID::CHANGE_PASSWORD, args_size, args,
    user_username_string, 0, 0, DIRECTORY_SERVICE_USERNAME, 0, 0);
    delete [] args;

    Message * change_password_reply;
    try {
        change_password_reply = myClient->sendMessage(change_password_msg, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(change_password_msg != NULL) {
            delete change_password_msg;
            change_password_msg = NULL;
        }
        throw except;
    } catch(const UnknownUsernameException& except) {
        if(change_password_msg != NULL) {
            delete change_password_msg;
            change_password_msg = NULL;
        }
        throw except;
    } catch(const SendingFailedException& except) {
        if(change_password_msg != NULL) {
            delete change_password_msg;
            change_password_msg = NULL;
        }
        throw except;
    }

    if(change_password_msg != NULL) {
        delete change_password_msg;
        change_password_msg = NULL;
    }

    if(change_password_reply == NULL) {
        ErrorHandler::displayWarning("Failed to change password: ERROR NULL");
        return false;
    }

    uint32_t reply_size = change_password_reply->getArgumentsSize();
    if(reply_size == 0) {
        ErrorHandler::displayWarning("Failed to change password: Reply arguments size is 0");
        delete change_password_reply;
        return false;
    } else {
        char * buffer = new char[reply_size];
        change_password_reply->getArguments(buffer, reply_size);
        delete change_password_reply;

        bool result = false;
        API::changePassword_arrayToResults(buffer, result);
        delete [] buffer;

        if(result) {
            myClient->setMyPassword(new_password);
        }

        return result;
    }
}


bool proxyFunctions::getAllUsers(Client* myClient, std::vector<std::string>& usernames) {
    uint32_t args_size;

    char * args = API::getUsers_argumentsToArray(myClient->getMyPassword(), args_size);

    Message * getAllUsers_msg = new Message(MessageType::Request, 0, OperationID::GET_USERNAMES, args_size, args,
    "", 0, 0, DIRECTORY_SERVICE_USERNAME, 0, 0);
    delete [] args;

    Message * getAllUsers_reply;
    try {
        getAllUsers_reply = myClient->sendMessage(getAllUsers_msg, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(getAllUsers_msg != NULL) {
            delete getAllUsers_msg;
            getAllUsers_msg = NULL;
        }
        throw except;
    } catch(const UnknownUsernameException& except) {
        if(getAllUsers_msg != NULL) {
            delete getAllUsers_msg;
            getAllUsers_msg = NULL;
        }
        throw except;
    } catch(const SendingFailedException& except) {
        if(getAllUsers_msg != NULL) {
            delete getAllUsers_msg;
            getAllUsers_msg = NULL;
        }
        throw except;
    }

    if(getAllUsers_msg != NULL) {
        delete getAllUsers_msg;
        getAllUsers_msg = NULL;
    }

    if(getAllUsers_reply == NULL) {
        ErrorHandler::displayWarning("Failed to get usernames: ERROR NULL");
        return false;
    } else {
        uint32_t reply_size = getAllUsers_reply->getArgumentsSize();
        if(reply_size == 0) {
            ErrorHandler::displayWarning("Failed to get usernames: Reply arguments size is 0");
            delete getAllUsers_reply;
            return false;
        }

        char * buffer = new char[reply_size];
        getAllUsers_reply->getArguments(buffer, reply_size);
        delete getAllUsers_reply;

        API::getUsers_arrayToResults(buffer, usernames);
        delete [] buffer;

        return true;
    }
}

bool proxyFunctions::offerServices(Client * myClient, std::vector<ImageView> arr, std::string image_name) {
	uint32_t args_size;
	
    char * args = API::allowUsertoView_argumentsToArray(myClient->getMyPassword(), arr, image_name, args_size);
	
    Message * offerServices_msg = new Message(MessageType::Request, 0, OperationID::OFFER_SERVICE, args_size, args,
    "", 0, 0, DIRECTORY_SERVICE_USERNAME, 0, 0);
    delete [] args;

    Message * offerServices_reply;
    try {
        offerServices_reply = myClient->sendMessage(offerServices_msg, timeout);

    } catch(const DirectoryServiceUnreachableException& except) {
        if(offerServices_msg != NULL) {
            delete offerServices_msg;
            offerServices_msg = NULL;
        }
        throw except;
    } catch(const UnknownUsernameException& except) {
        if(offerServices_msg != NULL) {
            delete offerServices_msg;
            offerServices_msg = NULL;
        }
        throw except;
    } catch(const SendingFailedException& except) {
        if(offerServices_msg != NULL) {
            delete offerServices_msg;
            offerServices_msg = NULL;
        }
        throw except;
    }

    if(offerServices_msg != NULL) {
        delete offerServices_msg;
        offerServices_msg = NULL;
    }

    if(offerServices_reply == NULL) {
		ErrorHandler::displayWarning("Failed to offerServices: ERROR NULL");
        return false;
    } else {
        delete offerServices_reply;
        return true;
    }
}

bool proxyFunctions::retrieveServices(Client * myClient, std::vector<ImageReference>& available_images) {
	uint32_t args_size;

    char * args = API::retreiveImages_arguemntsToArray(myClient->getMyPassword(), args_size);
		
    Message * retrieveServices_msg = new Message(MessageType::Request, 0, OperationID::GET_AVAILABLE_SERVICES, args_size, args,
    "", 0, 0, DIRECTORY_SERVICE_USERNAME, 0, 0);
    delete [] args;

    Message * retrieveServices_reply;
    try {
        retrieveServices_reply = myClient->sendMessage(retrieveServices_msg, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(retrieveServices_msg != NULL) {
            delete retrieveServices_msg;
            retrieveServices_msg = NULL;
        }
        throw except;
    } catch(const UnknownUsernameException& except) {
        if(retrieveServices_msg != NULL) {
            delete retrieveServices_msg;
            retrieveServices_msg = NULL;
        }
        throw except;
    } catch(const SendingFailedException& except) {
        if(retrieveServices_msg != NULL) {
            delete retrieveServices_msg;
            retrieveServices_msg = NULL;
        }
        throw except;
    }

    if(retrieveServices_msg != NULL) {
        delete retrieveServices_msg;
        retrieveServices_msg = NULL;
    }

    if(retrieveServices_reply == NULL) {
        ErrorHandler::displayWarning("Failed to retrieveServices: ERROR NULL");
        return false;
    } else {
        uint32_t reply_size = retrieveServices_reply->getArgumentsSize();
        if(reply_size == 0) {
            ErrorHandler::displayWarning("Failed to retrieveServices: Reply arguments size is 0");
            delete retrieveServices_reply;
            return false;
        }

        char * buffer = new char[reply_size];
        retrieveServices_reply->getArguments(buffer, reply_size);
        delete retrieveServices_reply;

        API::retreiveImages_arrayToResults(buffer, available_images);
        delete [] buffer;
        return true;
    }
}

bool proxyFunctions::logout(Client* myClient) {
    Message * logout_info = new Message(MessageType::Request, 0, OperationID::LOGOUT, 0, NULL,
    "", 0, 0, "", 0, 0);

    Message * logout_reply;
    try {
        logout_reply = myClient->sendMessage(logout_info, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(logout_info != NULL) {
            delete logout_info;
            logout_info = NULL;
        }
        throw except;
    } catch(const UnknownUsernameException& except) {
        if(logout_info != NULL) {
            delete logout_info;
            logout_info = NULL;
        }
        throw except;
    } catch(const SendingFailedException& except) {
        if(logout_info != NULL) {
            delete logout_info;
            logout_info = NULL;
        }
        throw except;
    }

    if(logout_info != NULL) {
        delete logout_info;
        logout_info = NULL;
    }


    if(logout_reply ==  NULL) {
        ErrorHandler::displayWarning("Failed to logout: ERROR NULL");
        return false;
    } else {
        delete logout_reply;
        return true;
    }
}

bool proxyFunctions::requestService_client(Client* myClient, ImageReference image_ref, uint32_t views_no, RemoteImageReference& RIR) {
    RIR.img_ref.image_name = "";
    RIR.img_ref.owner_username = "";
    RIR.shared_with_username = "";

    std::string image_string;
    uint32_t args_request_size;
    std::cout << "We are requesting the image " << image_ref.image_name << " from " << image_ref.owner_username << std::endl;
    char * args_request = API::requestImage_argumentsToArray(image_ref, views_no, args_request_size);

    Message * requestServices_msg = new Message(MessageType::Request, 0, REQUEST_IMAGE, args_request_size, args_request, user_username_string, 0, 0, image_ref.owner_username, 0, 0);
    delete [] args_request;

    std::cout << "Sending the request now" << std::endl;

    Message * requestServices_reply;
    try {
        requestServices_reply = myClient->sendMessage(requestServices_msg, SEND_IMAGE_TIMEOUT_USEC);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(requestServices_msg != NULL) {
            delete requestServices_msg;
            requestServices_msg = NULL;
        }
        throw except;
    } catch(const UnknownUsernameException& except) {
        if(requestServices_msg != NULL) {
            delete requestServices_msg;
            requestServices_msg = NULL;
        }
        throw except;
    } catch(const SendingFailedException& except) {
        if(requestServices_msg != NULL) {
            delete requestServices_msg;
            requestServices_msg = NULL;
        }
        throw except;
    }

    if(requestServices_msg != NULL) {
        delete requestServices_msg;
        requestServices_msg = NULL;
    }

    if(requestServices_reply== NULL) {
        ErrorHandler::displayWarning("Failed to requestService: ERROR NULL");
        return false;
    } else {
        std::cout << "A reply has arrived" << std::endl;
        uint32_t reply_size = requestServices_reply->getArgumentsSize();
        std::cout << "The reply message has " << reply_size << " bytes of arguments" << std::endl;
        char * buffer = new char[reply_size];
        requestServices_reply->getArguments(buffer, reply_size);
        delete requestServices_reply;

       API::replyWithRIRAck_arrayToArguments(buffer, RIR, views_no);
       delete [] buffer;
       return true;
    }
}

// aya!!!!!!!!!!!!!!!!!!!!!!!!!
/////////////////////////////////////////////////////////////////////////////////////////////
bool proxyFunctions::roam_service_client(const std::string& _my_username, InternetAddress _my_address, InternetAddress _my_server_address, InternetAddress _directory_service_address, std::string password)
{
    Client* myClient_;

    try {
        myClient_ = new Client(_my_username, _my_address, _my_server_address, _directory_service_address);
    } catch(const SocketBindingFailedException& except) {
        std::cout << except.what() << std::endl;
        return false;
    } catch(const SocketCreationFailedException& except) {
        std::cout << except.what() << std::endl;
        return false;
    }

    myClient_->setMyPassword(password);


    while(true)
    {

        //////////////////////////////////////////
        std::vector<RemoteImageReference>accepted_requests;
        std::vector<uint32_t>agreed_no_views;
        std::vector<Log_entry> statisticsLog;

        accepted_requests.resize(0);
        agreed_no_views.resize(0);
        statisticsLog.resize(0);


        any_log->read_accepted_requests(accepted_requests,agreed_no_views);

        Log_entry temp_log;

        for(int i = 0; i < accepted_requests.size() ; i++)
        {
            temp_log.shared_with_username =accepted_requests[i].shared_with_username;

            ImageQuota temp_quota;

            temp_quota.agreed_quota = agreed_no_views[i];
            temp_quota.img = accepted_requests[i].img_ref;
            temp_log.shared_with_images = temp_quota;
            statisticsLog.push_back(temp_log);
        }

        /////////////////////////////////////////


        Message* reply;
        std::vector<Statistic> views_of_person;
        views_of_person.resize(0);

        uint32_t reply_args_size;
        uint32_t reply_buffer_size;

        // 1st step will attempt to read from the file if it already exists



        std::map< std::string, std::vector<ImageUsersViews> > map_images_to_users_views;
        map_images_to_users_views.clear();
        std::cout<< "vector of log entry size: " << statisticsLog.size() << std::endl;

        for(int i = 0; i < statisticsLog.size(); i++)   // 1 iteration is for 1 shared_with_username
        {
            Message* request_statistics = new Message(Request, 0, OperationID::RETREIVE_STATISTICS, 0, NULL,
                                                      "", 0, 0,
                                                      statisticsLog[i].shared_with_username, 0, 0);
            std::cout<< "Before sending message of statistics: " << std::endl;

            reply = NULL;

            try {
                reply = myClient_->sendMessage(request_statistics, DEFAULT_TIMEOUT_USEC);
            } catch(const DirectoryServiceUnreachableException& except) {
                if(request_statistics != NULL) {
                    delete request_statistics;
                    request_statistics = NULL;
                }

                std::cout << except.what() << std::endl;
            } catch(const UnknownUsernameException& except) {
                if(request_statistics != NULL) {
                    delete request_statistics;
                    request_statistics = NULL;
                }

                std::cout << except.what() << std::endl;
            } catch(const SendingFailedException& except) {
                if(request_statistics != NULL) {
                    delete request_statistics;
                    request_statistics = NULL;
                }

                std::cout << except.what() << std::endl;
            }

            if(request_statistics != NULL) {
                delete request_statistics;
                request_statistics = NULL;
            }


            std::cout<< "After sending message of statistics: " << std::endl;


            if(reply == NULL)
            {
                std::cout<< "Reply is null " <<std::endl;
                ErrorHandler::displayWarning("Failed to get statistics: ERROR NULL");

                std::map< std::string, std::vector<ImageUsersViews> > old_map_images_to_users_views;
                old_map_images_to_users_views.clear();

                 if(any_log->robot_read_statistics (old_map_images_to_users_views))// if it successfully read the file
                 {
                     for(std::map<std::string,std::vector<ImageUsersViews>>::iterator it = old_map_images_to_users_views.begin(); it != old_map_images_to_users_views.end(); ++it)
                        {
                            // it->first is the key and it->second is the value
                            for(int j = 0; j < it->second.size(); j++)   // loop on the number of users that I shared this image with
                            {
                                if(it->second[j].shared_with_username == statisticsLog[i].shared_with_username)  // check if the username of this offline person is in this image
                                {
                                    ImageUsersViews offline_person;
                                    offline_person.shared_with_username = it->second[j].shared_with_username ;
                                    offline_person.actual_views = old_map_images_to_users_views[it->first][j].actual_views;
                                    map_images_to_users_views[it->first].push_back(offline_person);
                                }
                            }

                        }

                 }


    //            // I need to write the name of this user for all his images

    //            std::map< std::string, std::vector<ImageUsersViews> > old_map_images_to_users_views;

    //            any_log->robot_read_statistics (old_map_images_to_users_views);// key is imageName, value is vector containing

    //            // need to get all images in this map that this offline user have from me




    //            for(std::map<std::string,std::vector<ImageUsersViews>>::iterator it = old_map_images_to_users_views.begin(); it != old_map_images_to_users_views.end(); ++it)
    //            {
    //                // it->first is the key and it->second is the value
    //                for(int j = 0; j < it->second.size(); j++)   // loop on the number of users that I shared this image with
    //                {
    //                    if(it->second[j].shared_with_username == myLog[i].shared_with_username)  // check if the username of this offline person is in this image
    //                    {
    //                        ImageUsersViews offline_person;
    //                        offline_person.shared_with_username = it->second[j].shared_with_username ;
    //                        offline_person.actual_views = old_map_images_to_users_views[it->first][j].actual_views;
    //                        map_images_to_users_views[it->first].push_back(offline_person);
    //                    }
    //                }

    //            }

            }

            else
            {
                //std::cout<< "After sending message of statistics: " << std::endl;
                std::cout<< "Reply of statistics size is " <<  reply->getArgumentsSize() <<std::endl;

                std::cout <<"Printing output" <<std::endl;
                reply->print();
                reply_buffer_size = reply->getArgumentsSize();
                std::cout<< "Reply is not null: "<< reply_buffer_size <<std::endl;
                char* reply_buffer = new char[reply_buffer_size];
                reply->getArguments(reply_buffer, reply_buffer_size);

                // unmarshal the reply_buffer
                std::cout<< "Got arguments from reply: " << reply_buffer <<std::endl;
                API::retrievedStatistics_arrayToArguments(reply_buffer, reply_buffer_size , views_of_person);
                std::cout<< "vecrtor of statistics size: " << views_of_person.size() << std::endl;

                for(unsigned int j = 0 ; j < views_of_person.size(); j++ )
                {
                    std::string img_name = views_of_person[j].image_name;
                    ImageUsersViews username_views;
                    username_views.shared_with_username = statisticsLog[i].shared_with_username;
                    username_views.actual_views = views_of_person[j].views_no;

                    map_images_to_users_views[img_name].push_back(username_views);


    //                if(map_images_to_users_views.find(img_name) == map_images_to_users_views.end())  // this image isn't existing before
    //                {
    //                    // 1st time to get this image so declare a new vector
    //                    std::vector<ImageUsersViews> new_vector;
    //                    new_vector.resize(0);
    //                    new_vector.push_back(username_views);
    //                    map_images_to_users_views[img_name] = new_vector;
    //                } else {
    //                    map_images_to_users_views[img_name].push_back(username_views);
    //                }

                }

                delete [] reply_buffer;
            }


        }



        any_log->robot_write_statistics (map_images_to_users_views);// key is imageName, value is vector containing

        std::this_thread::sleep_for(std::chrono::seconds(ROBOT_SLEEP_SECONDS));

    }



    delete myClient_;

    return true;

}
///////////////////////////////////////////////////////////////////////////////////////////
// ayaaa!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


bool proxyFunctions::grantImage(Client * myClient, RemoteImageReference RIR, uint32_t views_no) {
    uint32_t args_size;
    char * args;

    std::string image_string;
    image::prepareImageToSend(owned_images_path,"local_"+RIR.img_ref.image_name,views_no,RIR.shared_with_username,image_string);

    args = API::replyWithImage_argumentsToArray(image_string, RIR, args_size);

    std::cout << "Attempt to send image" << std::endl;

    Message * grantImage_msg = new Message(MessageType::Request, 0, OperationID::SEND_IMAGE, args_size, args,
                                           RIR.img_ref.owner_username, 0, 0, RIR.shared_with_username, 0, 0);
    delete [] args;


    Message * grantImage_reply;
    try {
        grantImage_reply = myClient->sendMessage(grantImage_msg, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(grantImage_msg != NULL) {
            delete grantImage_msg;
            grantImage_msg = NULL;
        }

        throw except;
    } catch(const UnknownUsernameException& except) {
        if(grantImage_msg != NULL) {
            delete grantImage_msg;
            grantImage_msg = NULL;
        }

        throw except;
    } catch(const SendingFailedException& except) {
        if(grantImage_msg != NULL) {
            delete grantImage_msg;
            grantImage_msg = NULL;
        }

        throw except;
    }

    if(grantImage_msg != NULL) {
        delete grantImage_msg;
        grantImage_msg = NULL;
    }



    if(grantImage_reply == NULL) {

        ErrorHandler::displayWarning("Failed to grantImage: ERROR NULL");

        return false;

    } else {
        std::cout << "Image sent" << std::endl;

        delete grantImage_reply;

        //write to  file accepted log

        any_log->write_accepted_requests(RIR, views_no);

        return true;

    }

}


bool proxyFunctions::changeQouta(Client* myClient, PendingQuotaChange& change) {
    uint32_t args_size;
    char * args = API::updateViews_argumentsToArray(change, args_size);

    Message * changeQouta_msg = new Message(MessageType::Request, 0, OperationID::UPDATE_VIEWS_COUNTER
                                            , args_size, args,
                                            change.remote_img_ref.img_ref.owner_username, 0, 0,
                                            change.remote_img_ref.shared_with_username, 0, 0);

    delete [] args;

    Message * changeQouta_reply;


    try {
        changeQouta_reply = myClient->sendMessage(changeQouta_msg, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(changeQouta_msg != NULL) {
            delete changeQouta_msg;
            changeQouta_msg = NULL;
        }

        throw except;
    } catch(const UnknownUsernameException& except) {
        if(changeQouta_msg != NULL) {
            delete changeQouta_msg;
            changeQouta_msg = NULL;
        }

        throw except;
    } catch(const SendingFailedException& except) {
        if(changeQouta_msg != NULL) {
            delete changeQouta_msg;
            changeQouta_msg = NULL;
        }

        throw except;
    }



    if(changeQouta_reply == NULL) {

        ErrorHandler::displayWarning("Failed to changeQouta: ERROR NULL");

        return false;

    } else {

        delete changeQouta_reply;
        any_log->update_accepted_requests(change.remote_img_ref, change.new_quota);
        return true;

    }

}

bool proxyFunctions::request_more_views(Client* myClient, ImageReference& img_ref, uint32_t views_no) {
    uint32_t args_size;
    char * args = API::requestviews_argumentsToArray(img_ref, views_no , args_size);

    Message * request_more_views_msg = new Message(MessageType::Request, 0, OperationID::REQUEST_ADDITIONAL_VIEWS
                                            , args_size, args,
                                            myClient->my_username, 0, 0,
                                            img_ref.owner_username, 0, 0);
    delete [] args;

    Message * request_more_views_reply;

    try {
        request_more_views_reply = myClient->sendMessage(request_more_views_msg, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(request_more_views_msg != NULL) {
            delete request_more_views_msg;
            request_more_views_msg = NULL;
        }

        throw except;
    } catch(const UnknownUsernameException& except) {
        if(request_more_views_msg != NULL) {
            delete request_more_views_msg;
            request_more_views_msg = NULL;
        }

        throw except;
    } catch(const SendingFailedException& except) {
        if(request_more_views_msg != NULL) {
            delete request_more_views_msg;
            request_more_views_msg = NULL;
        }

        throw except;
    }

    if(request_more_views_reply == NULL) {
        ErrorHandler::displayWarning("Failed to request_more_views: ERROR NULL");
        return false;
    } else {
        delete request_more_views_reply;
        return true;
    }
}

bool proxyFunctions::announceImage(Client* myClient, std::string image_name) {
    uint32_t args_size;

    char * args = API::announceImage_argumentsToArray(myClient->getMyPassword(), image_name,args_size);

    Message * announceImage_msg = new Message(MessageType::Request, 0, OperationID::ANNOUNCE_IMAGE, args_size, args,
    "", 0, 0, DIRECTORY_SERVICE_USERNAME, 0, 0);
    delete [] args;

    Message * announceImage_reply;

    try {
        announceImage_reply = myClient->sendMessage(announceImage_msg, timeout);
    } catch(const DirectoryServiceUnreachableException& except) {
        if(announceImage_msg != NULL) {
            delete announceImage_msg;
            announceImage_msg = NULL;
        }

        throw except;
    } catch(const UnknownUsernameException& except) {
        if(announceImage_msg != NULL) {
            delete announceImage_msg;
            announceImage_msg = NULL;
        }

        throw except;
    } catch(const SendingFailedException& except) {
        if(announceImage_msg != NULL) {
            delete announceImage_msg;
            announceImage_msg = NULL;
        }

        throw except;
    }

    if(announceImage_reply == NULL) {
        ErrorHandler::displayWarning("Failed to announce image: ERROR NULL");
        return false;
    } else {
        delete announceImage_reply;
        return true;
    }
}
