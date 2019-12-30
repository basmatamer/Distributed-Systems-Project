#ifndef IMAGE_H
#define IMAGE_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

class image {
public:
//    static void saveImg(string pic, string usernameSender, string usernameReceiver,string originalName);

//    image(string sender_username, string receiver_username, string originalName);
  //  image(string sender_username, string image_name);

    static void prepareImageToSend(string folder_path, string big_image, int number_of_views, string receiver_username, string& prepared_image);

    static string owner_click_extract_image(string folder_path, string image_name);
    static string receiver_click_extract_image(string folder_path, string image_name, int& remaining_views);
    static int getViewsForRobot(std::string folder_path, std::string image_name);
    static void updateQuota(std::string folder_path, std::string image_name, int new_quota);
    static void copyImg(string path, string org_name, string new_img);
    static void embedStegImg(string path, string large_img, string small_img);

    static void textToImg(string path, string image, string s); //path to save new img to
    static string imgToText(string path, string image);




  //  void sendImg(string viewsNo,string receiver);
   // void extractImgFromImg();
  //  string extractTextFromImg();

//    string getHiddenImageString();
//    void setMaximumViews(int newMaximumViews);
//    int getRemainingViews();
//    int getMaximumViews();
//    int getActualViews();

//    string extractImg();
//    ~image(); //DONT FORGET

//    string getImgString();


private:
//    string owned_images_path="/home/basma/Desktop/SharingImages/basma/ownedResources/Images/";
//    string shared_images_path="/home/basma/Desktop/SharingImages/tawfik/sharedResources/Images/basma/";
//    string DEFAULT_IMAGE_NAME="def.jpg";
//    string owner_username="basma";


    static void embedSteg(string path, string image, string s);
    static string extractSteg(string path, string image_name);

    static void extractStegImg(string path, string image, string original_image_name);

//    std::string getQuotaFileName();

//    string /*owner_username,*/ receiver_username, image_name, original_image_name;
//    bool isSender;
};

#endif // IMAGE_H
