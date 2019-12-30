#ifndef proxyFunctions_hpp
#define proxyFunctions_hpp

#include <stdio.h>
#include "client.hpp"
#include "Server.hpp"
#include "Message.hpp"
#include "globalvar.h"

#include <vector>

class proxyFunctions {
	private:
		
	public:
        static bool signup(Client* myClient,std::string username, std::string password, std::string& display_msg);
        static bool login(Client* myClient,std::string username, std::string password);
        static void hiServer(Client* myClient, InternetAddress server_address);
        static bool removePending(Client * myClient);
        static bool getAllUsers(Client* myClient,std::vector<std::string>& usernames);
        static bool changePassword(Client* myClient, std::string old_password, std::string new_password);
        static bool offerServices(Client* myClient,std::vector<ImageView> arr, std::string image_name);
        static bool retrieveServices(Client* myClient,std::vector<ImageReference>& available_images);
        static bool logout(Client* myClient);
        static bool requestService_client(Client* myClient, ImageReference image_ref, uint32_t views_no, RemoteImageReference& RIR);
//		static void changeQouta(Client* myClient,ImageReference& image, uint32_t views_no);

        // aya!!!!!!!!!!!!!!!!!!!!11
        static bool roam_service_client(const std::string& _my_username, InternetAddress _my_address, InternetAddress _my_server_address, InternetAddress _directory_service_address, std::string password);



        static bool grantImage(Client * myClient, RemoteImageReference RIR, uint32_t views_no);
        static bool changeQouta(Client* myClient, PendingQuotaChange& change);
        static bool request_more_views(Client* myClient, ImageReference& image, uint32_t views_no);

        static bool announceImage(Client* myClient, std::string image_name);

};

#endif /* proxyFunctions_hpp */
