#include "image.h"
#include <string>
#include <fstream>

using namespace std;

string image::imgToText(string path, string image) {
    string final_path = path + image;
    ostringstream os;
    ifstream file(final_path);
    os << file.rdbuf();

    string s = os.str();

    return s;
}

void image::textToImg(string path, string image, string s) {
    string final_path = path + image;
    ofstream m_OutFile;
    m_OutFile.open(final_path.c_str(), std::ios::out);
    ostringstream oStr;
    oStr << s << std::ends;
    m_OutFile << oStr.str() <<endl;
}

void image::embedSteg(string path, string image, string s) {
    string command="steghide embed -cf " + path + image + " -ef " + path + s + " -p 123 ";
    system(command.c_str());
}

void image::copyImg(string path, string org_name, string new_img) {
    std::ifstream input ((path + org_name).c_str(), std::ios::binary);
    std::ofstream output ((path + new_img).c_str(), std::ios::binary);
    output << input.rdbuf();
}


void image:: embedStegImg(string path, string large_image, string small_image) {
    string command="steghide embed -cf " + path + large_image + " -ef " + path + small_image + " -p 123";
    system(command.c_str());
}

string image::extractSteg(string path, string image_name) {
   string command= "steghide extract -p 123 -sf " + path + image_name;
   system(command.c_str());

   std::string quota_file_name = image_name.substr(0, image_name.find_last_of('.')) + "_quota.txt";

   std::rename(quota_file_name.c_str(), (path+quota_file_name).c_str());

   string line = "0 0";
   ifstream myfile (path+quota_file_name);
   if (myfile.is_open()) {
        getline (myfile,line);
        myfile.close();
   }
   else {
       cout << "Unable to open file" << std::endl;
   }
    return line;
}

void image::extractStegImg(string path, string image, string original_image_name) {
    string command = "steghide extract -sf " + path + image + " -p 123";
    system(command.c_str());
    std::rename(original_image_name.c_str(), (path + original_image_name).c_str());
}


void image::prepareImageToSend(string folder_path, string big_image, int number_of_views, string receiver_username, string& prepared_image)
{
    // copy the hidden image to share to not mess with owner's image
    std::string receiver_big_image = receiver_username + "_" + big_image;
    copyImg(folder_path, big_image, receiver_big_image);

    // extract the owner's image itself
    string original_imported_image = big_image.substr(big_image.find("_") + 1, big_image.length());
    extractStegImg(folder_path, receiver_big_image, original_imported_image);

    // Rename the extracted owner's image to not get confused between different threads
    string receiver_original_imported_image = receiver_username + "_" + original_imported_image;
    std::rename((folder_path + original_imported_image).c_str(), (folder_path + receiver_original_imported_image).c_str());

    // Create quota file for this receiver for this image
    std::ofstream quota_output;
    std::string quota_file_name = receiver_original_imported_image.substr(0, receiver_original_imported_image.find_last_of('.')) + "_quota.txt";
    quota_output.open(folder_path + quota_file_name);
    quota_output << number_of_views << " 0" << std::endl;
    quota_output.close();

    // Hide the quota file in the receiver original imported image
    embedSteg(folder_path, receiver_original_imported_image, quota_file_name);

    // Remove quota file
    std::remove((folder_path + quota_file_name).c_str());

    // Embed small image with quota into large image of receiver
    embedStegImg(folder_path, receiver_big_image, receiver_original_imported_image);

    // Remove the extracted image of sender
    std::remove((folder_path + receiver_original_imported_image).c_str());

    // Flatten the image
    prepared_image = imgToText(folder_path, receiver_big_image);

    // Delete the file we prepared (we already have it as string)
    std::remove((folder_path + receiver_big_image).c_str());
}

string image::owner_click_extract_image(string folder_path, string image_name)
{
    string path_to_return;
    string original_imported_image = image_name.substr(image_name.find("_") + 1, image_name.length());

    string name_to_use = "temp_" + image_name;
    copyImg(folder_path, image_name, name_to_use);

    extractStegImg(folder_path, name_to_use, original_imported_image);
    std::remove((folder_path + name_to_use).c_str());

    path_to_return = folder_path + original_imported_image;

    return path_to_return;
}

string image::receiver_click_extract_image(string folder_path, string image_name, int& remaining_views) {
    string path_to_return;
    string original_sent_image = image_name.substr(image_name.find("_") + 1, image_name.length());

    extractStegImg(folder_path, image_name, original_sent_image);


    std::string current_quota = extractSteg(folder_path, original_sent_image);
    int total_quota = std::stoi(current_quota.substr(0, current_quota.find(' ')));
    int current_views = std::stoi(current_quota.substr(current_quota.find(' ') + 1, current_quota.length()));

    remaining_views = std::max(total_quota - current_views, 0);
    if(remaining_views > 0) {
        path_to_return = folder_path + original_sent_image;
        current_quota = std::to_string(total_quota) + " " + std::to_string(current_views + 1);
    } else {
        path_to_return = folder_path + image_name;
    }

    // Create quota file for this receiver for this image
    std::ofstream quota_output;
    std::string quota_file_name = original_sent_image.substr(0, original_sent_image.find_last_of('.')) + "_quota.txt";
    quota_output.open(folder_path + quota_file_name);
    quota_output << current_quota << std::endl;
    quota_output.close();

    // Hide the quota file in the receiver original imported image
    embedSteg(folder_path, original_sent_image, quota_file_name);

    // Hide the shared image back to the default image
    embedStegImg(folder_path, image_name, original_sent_image);


    std::remove((folder_path + quota_file_name).c_str());
    if(remaining_views <= 0) {
        std::remove((folder_path + original_sent_image).c_str());
    }

    return path_to_return;
}

int image::getViewsForRobot(std::string folder_path, std::string image_name) {
    string original_sent_image = image_name.substr(image_name.find("_") + 1, image_name.length());

    string name_to_use = "temp_" + image_name;
    copyImg(folder_path, image_name, name_to_use);

    extractStegImg(folder_path, name_to_use, original_sent_image);

    std::string current_quota = extractSteg(folder_path, original_sent_image);
    int total_quota = std::stoi(current_quota.substr(0, current_quota.find(' ')));
    int current_views = std::stoi(current_quota.substr(current_quota.find(' ') + 1, current_quota.length()));

    std::remove((folder_path + name_to_use).c_str());
    std::remove((folder_path + original_sent_image).c_str());
    std::string quota_file_name = original_sent_image.substr(0, original_sent_image.find_last_of('.')) + "_quota.txt";
    std::remove((folder_path + quota_file_name).c_str());

    return current_views;
}

void image::updateQuota(std::string folder_path, std::string image_name, int new_quota) {
    string original_sent_image = image_name.substr(image_name.find("_") + 1, image_name.length());

    extractStegImg(folder_path, image_name, original_sent_image);


    std::string current_quota = extractSteg(folder_path, original_sent_image);

    std::cout << "The old quota string is " << current_quota << std::endl;

    // int total_quota = std::stoi(current_quota.substr(0, current_quota.find(' ')));
    int current_views = std::stoi(current_quota.substr(current_quota.find(' ') + 1, current_quota.length()));

    current_quota = std::to_string(new_quota) + " " + std::to_string(current_views);

    std::cout << "The new quota string is " << current_quota << std::endl;

    // Create quota file for this receiver for this image
    std::ofstream quota_output;
    std::string quota_file_name = original_sent_image.substr(0, original_sent_image.find_last_of('.')) + "_quota.txt";
    quota_output.open(folder_path + quota_file_name);
    quota_output << current_quota << std::endl;
    quota_output.close();

    // Hide the quota file in the receiver original imported image
    embedSteg(folder_path, original_sent_image, quota_file_name);

    // Hide the shared image back to the default image
    embedStegImg(folder_path, image_name, original_sent_image);

    std::cout << "Attempting to remove " << folder_path + original_sent_image << std::endl;
    std::cout << "Attempting to remove " << folder_path + quota_file_name << std::endl;

    std::remove((folder_path + original_sent_image).c_str());
    std::remove((folder_path + quota_file_name).c_str());
}


