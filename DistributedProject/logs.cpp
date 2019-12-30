#include "logs.h"

logs::logs()
{

}

void logs::robot_write_statistics (std::map<std::string , std::vector<ImageUsersViews> > new_robot_results) // key is imageName, value is vector containing
                                                                        // sharedWith username and currentViews
{
    // 1st line contains number of images
    // 2nd line contains ith image name
    // 3rd line contains number of users that I shared this image with
    // 4th line contains username then space then his views_no

    // then i+1th image name and so on...


    // NEWWWWW!!!!!!!!!!!!!!!!!!!!!!1

    mutex_statistics.lock();
    std::lock_guard<std::mutex> lk_statistics(mutex_statistics, std::adopt_lock);

    std::ofstream output_myfile;
    std::vector<std::string> all_images_names;   // vector of keys
    all_images_names = get_keys_of_map(new_robot_results);

    output_myfile.open(STATISTICS_LOG_FILENAME);

    if(output_myfile.is_open())
    {
        std::cout<<"images size: "<< all_images_names.size() <<std::endl;
        output_myfile << all_images_names.size() <<std::endl;   // number of images

        for(int i = 0; i < all_images_names.size(); i++)
        {
            output_myfile << all_images_names[i].substr(all_images_names[i].find_last_of('_')+1, all_images_names[i].length()) <<std::endl; // name of image
            output_myfile << new_robot_results[all_images_names[i]].size() << std::endl;

            for(int m = 0; m < new_robot_results[all_images_names[i]].size(); m++)   // number of users at this image
            {

                std::cout<<"shared with username: "<< new_robot_results[all_images_names[i]][m].shared_with_username <<std::endl;
                std::cout<<"actual views: "<< new_robot_results[all_images_names[i]][m].actual_views <<std::endl;
                output_myfile << new_robot_results[all_images_names[i]][m].shared_with_username + "\t";  // ith username
                output_myfile << new_robot_results[all_images_names[i]][m].actual_views <<std::endl;  // views of ith user
            }


        }


        output_myfile.close();
    }
    else

    {
        std::cout << "Cannot open file for statistics" << std::endl;
    }

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1



//    std::ifstream input_myfile (STATISTICS_LOG_FILENAME);    // file_name here should be :  "C:/OurApp/CacheOfStatistics/myusername.txt"

//    if (input_myfile.is_open())   // if the file already exists read statistics kept in it
//    {
//        std::map< std::string , std::vector<ImageUsersViews> > old_robot_results;

//        int number_of_images;
//        input_myfile >> number_of_images;

//        for(int i = 0; i < number_of_images; i++)
//        {
//            std::string image_name;
//            input_myfile >> image_name;

//            //if(image_name == get_keys_of_map(old_robot_results)][0])

//            int number_of_users;
//            input_myfile >> number_of_users;

//            std::vector<ImageUsersViews> image_details;   // details of a sepcific image

//            for(int k = 0; k < number_of_users; k++)
//            {
//                std::string username;
//                int views_no;
//                input_myfile >> username;
//                input_myfile >> views_no;

//                ImageUsersViews details;   // details of a single user having this image
//                details.shared_with_username = username;
//                details.actual_views = views_no;

//                image_details.push_back(details);
//            }

//            old_robot_results[image_name] = image_details;

//        }


//        input_myfile.close();

//        // compare new_robot_results with old_robot_results and overwrite if needed

//        std::vector<std::string> all_new_images_names;
//        all_new_images_names = get_keys_of_map(new_robot_results);

//        for(int a = 0; a < all_new_images_names.size(); a++)    // loop to get details of a specific image
//        {
//            std::string specific_image = all_new_images_names[a];  // access the ath image

//            if(old_robot_results.find(specific_image) == old_robot_results.end()) // that image isn't aslan in the old file
//            {
//                old_robot_results[specific_image] = new_robot_results[specific_image];
//            }

//            else   // this image is already existing in the old_robot_results but let's check 3la kol el users
//            {
//                if(new_robot_results[specific_image].size() == old_robot_results[specific_image].size())   // the image is existing and has the same number of users
//                {
//                    for(int j = 0; j < new_robot_results[specific_image].size(); j++)   // loops on this vector
//                    {
//                        if(new_robot_results[specific_image][j].shared_with_username == old_robot_results[specific_image][j].shared_with_username)
//                            old_robot_results[specific_image][j].actual_views = new_robot_results[specific_image][j].actual_views;
//                    }

//                }

//                else if (new_robot_results[specific_image].size() > old_robot_results[specific_image].size())  // the new has more users
//                        {
//                            /// add the extra new usernames!!!
//                            // check this assumption eny ha add them fl a5r w 5las ??????
//                            for(int b  = old_robot_results[specific_image].size(); b < new_robot_results[specific_image].size(); b++)
//                                old_robot_results[specific_image].push_back(new_robot_results[specific_image][b]);

//                        }

//            }
//        }


//        // now write old_robot_results after the updates in a file
//        std::ofstream output_update_myfile;
//        std::vector<std::string> all_images_names;   // vector of keys
//        all_images_names = get_keys_of_map(old_robot_results);

//        output_update_myfile.open(STATISTICS_LOG_FILENAME, std::ios_base::trunc);

//        std::cout<<"images size: "<< all_images_names.size() <<std::endl;
//        output_update_myfile << all_images_names.size() <<std::endl;   // number of images

//        for(int i = 0; i < all_images_names.size(); i++)
//        {
//            output_update_myfile << all_images_names[i].substr(all_images_names[i].find_last_of('_')+1, all_images_names[i].length()) <<std::endl; // name of image
//            output_update_myfile << new_robot_results[all_images_names[i]].size() << std::endl;
//            for(int m = 0; m < new_robot_results[all_images_names[i]].size(); m++)   // number of users at this image
//            {
//                std::cout<<"shared with username: "<< new_robot_results[all_images_names[i]][m].shared_with_username <<std::endl;
//                std::cout<<"actual views: "<< new_robot_results[all_images_names[i]][m].actual_views <<std::endl;
//                output_update_myfile << new_robot_results[all_images_names[i]][m].shared_with_username + "\t";  // ith username
//                output_update_myfile << new_robot_results[all_images_names[i]][m].actual_views <<std::endl;  // views of ith user
//            }


//        }


//        output_update_myfile.close();


//            // either en feeh image gdeeda aslan f new statisitcs so lazim n7otaha
//            // aw yb2a feeh new added user 3la image already mawgooda
//            // aw el tab33ey eny ha update el views btoo3 user already existing !!

//            // el moshkla hat7sal lw user kan mawgood fl image f old bs magash f new 3shan el sha5s offline!!! sa3tha msh han overwrite


//            // bs limitation eno msh by ysheel image deleted mn ay 7d!!!!


//    }


//    else   // if this file isn't already existing, just create it and write the statistics to it
//    {
//        std::ofstream output_myfile;
//        std::vector<std::string> all_images_names;   // vector of keys
//        all_images_names = get_keys_of_map(new_robot_results);

//        output_myfile.open(STATISTICS_LOG_FILENAME);

//        if(output_myfile.is_open())
//        {
//            std::cout<<"images size: "<< all_images_names.size() <<std::endl;
//            output_myfile << all_images_names.size() <<std::endl;   // number of images

//            for(int i = 0; i < all_images_names.size(); i++)
//            {
//                output_myfile << all_images_names[i].substr(all_images_names[i].find_last_of('_')+1, all_images_names[i].length()) <<std::endl; // name of image
//                output_myfile << new_robot_results[all_images_names[i]].size() << std::endl;
//                for(int m = 0; m < new_robot_results[all_images_names[i]].size(); m++)   // number of users at this image
//                {

//                    std::cout<<"shared with username: "<< new_robot_results[all_images_names[i]][m].shared_with_username <<std::endl;
//                    std::cout<<"actual views: "<< new_robot_results[all_images_names[i]][m].actual_views <<std::endl;
//                    output_myfile << new_robot_results[all_images_names[i]][m].shared_with_username + "\t";  // ith username
//                    output_myfile << new_robot_results[all_images_names[i]][m].actual_views <<std::endl;  // views of ith user
//                }


//            }


//            output_myfile.close();
//        }
//        else

//        {
//            std::cout << "Cannot open file for statistics" << std::endl;
//        }

//    }

}




// ayaaaa!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

bool logs::robot_read_statistics(std::map<std::string , std::vector<ImageUsersViews> >& old_robot_results)
{
    mutex_statistics.lock();
    std::lock_guard<std::mutex> lk_statistics(mutex_statistics, std::adopt_lock);

    std::ifstream input_myfile (STATISTICS_LOG_FILENAME);    // file_name here should be :  "C:/OurApp/CacheOfStatistics/myusername.txt"

    bool success_read = false;

    if (input_myfile.is_open())   // if the file already exists read statistics kept in it
    {
        success_read = true;
        int number_of_images;
        input_myfile >> number_of_images;

        for(int i = 0; i < number_of_images; i++)
        {
            std::string image_name;
            input_myfile >> image_name;

            int number_of_users;
            input_myfile >> number_of_users;

            std::vector<ImageUsersViews> image_details;   // details of a sepcific image

            for(int k = 0; k < number_of_users; k++)
            {
                std::string username;
                int views_no;
                input_myfile >> username;
                input_myfile >> views_no;

                ImageUsersViews details;   // details of a single user having this image
                details.shared_with_username = username;
                details.actual_views = views_no;

                image_details.push_back(details);
            }

            old_robot_results[image_name] = image_details;

        }
        input_myfile.close();
    }


    return success_read;

}

std::vector<std::string> logs::get_keys_of_map(std::map< std::string , std::vector<ImageUsersViews> > m)
{
   std:: vector<std::string> v;
    for(std::map< std::string , std::vector<ImageUsersViews> >::iterator it = m.begin(); it != m.end(); ++it) {
        v.push_back(it->first);
    }
    return v;
}

void logs::write_pending_requests(RemoteImageReference RIR, uint32_t views_no)

{
    mutex_pending.lock();

    std::lock_guard<std::mutex> lk_pending(mutex_pending, std::adopt_lock);

    std::ofstream output;

    output.open(PENDING_REQUESTS_FILENAME, std::ios_base::app);

    if(output.is_open())

    {

        output << RIR.shared_with_username << " " << RIR.img_ref.image_name << " " << views_no << " " << RIR.img_ref.owner_username<< std::endl;

        output.close();

    }


}

void logs::read_pending_requests(std::vector<RemoteImageReference>& pending_requests, std::vector<uint32_t>& no_views)

{
    std::cout << "we are at x" << std::endl;

    mutex_pending.lock();
    std::cout << "we are at a" << std::endl;

    std::lock_guard<std::mutex> lk_pending(mutex_pending, std::adopt_lock);

    std::cout << "we are at b" << std::endl;


    std::ifstream input;

    no_views.clear();
    pending_requests.clear();

    std::cout << "The filename is " << PENDING_REQUESTS_FILENAME << std::endl;

    input.open(PENDING_REQUESTS_FILENAME);

    std::cout << "we are at d" << std::endl;

    if(input.is_open())
    {
        std::cout << "we are at e" << std::endl;

        RemoteImageReference temp;



        input>>temp.shared_with_username;

            while(!input.eof())

            {
                uint32_t views = 0;
                 std::cout << "shared with username " <<temp.shared_with_username <<std::endl;
                input>>temp.img_ref.image_name;
                std::cout << "image name: " <<temp.img_ref.image_name <<std::endl;
                input>>views;
                std::cout << "views " <<views <<std::endl;
                input>>temp.img_ref.owner_username;
                std::cout << "owner username " <<temp.img_ref.owner_username <<std::endl;
                pending_requests.push_back(temp);

                no_views.push_back(views);

               // std::cout <<"The file output is: " <<temp.shared_with_username + " " << temp.img_ref.image_name + " " << views + " " << temp.img_ref.owner_username<< std::endl;
                input>>temp.shared_with_username;

            }

            input.close();


    }
}

void logs::rewrite_pending_requests(std::vector<RemoteImageReference> pending_requests, std::vector<uint32_t> no_views)
{

    mutex_pending.lock();
    std::cout << "pending locked in rewrite" << std::endl;

    std::lock_guard<std::mutex> lk_pending(mutex_pending, std::adopt_lock);

    std::ofstream output;

    output.open(PENDING_REQUESTS_FILENAME, std::ios_base::trunc);

    if(output.is_open())

    {

        for(int i = 0; i< pending_requests.size(); i++)

            output << pending_requests.at(i).shared_with_username << " " << pending_requests.at(i).img_ref.image_name << " " << no_views.at(i) << " " << pending_requests.at(i).img_ref.owner_username << std::endl;


        output.close();

    }

    else

    {

        std::cout << "Cannot open file pending requests" << std::endl;

    }
}

void logs::write_accepted_requests(RemoteImageReference RIR, uint32_t views_no)

{
    mutex_accepted.lock();
    std::lock_guard<std::mutex> lk_accepted(mutex_accepted, std::adopt_lock);

    std::ofstream output;

    output.open(ACCEPTED_REQUESTS_FILENAME, std::ios_base::app);

    if(output.is_open())

    {

        output << RIR.shared_with_username << " " << RIR.img_ref.image_name << " " << views_no << " " << RIR.img_ref.owner_username << std::endl;

        output.close();

    }

}

void logs::read_accepted_requests(std::vector<RemoteImageReference>& accepted_requests, std::vector<uint32_t>& no_views)

{
    mutex_accepted.lock();
    std::lock_guard<std::mutex> lk_accepted(mutex_accepted, std::adopt_lock);

    accepted_requests.clear();
    no_views.clear();

    std::ifstream input;

    input.open(ACCEPTED_REQUESTS_FILENAME);

    if(input.is_open())

    {
        RemoteImageReference temp;
        input>>temp.shared_with_username;
        while(!input.eof())

        {
            uint32_t views;

            input>>temp.img_ref.image_name;

            input>>views;

            input>>temp.img_ref.owner_username;

            accepted_requests.push_back(temp);

            no_views.push_back(views);

            input>>temp.shared_with_username;

        }

        input.close();

    }



}

void logs::update_accepted_requests(RemoteImageReference RIR, uint32_t views_no)
{

    std::vector<RemoteImageReference> accepted_requests;
    std::vector<uint32_t> agreed_no_views;

    read_accepted_requests(accepted_requests, agreed_no_views);

    mutex_accepted.lock();
    std::lock_guard<std::mutex> lk_accepted(mutex_accepted, std::adopt_lock);

    for(int i = 0; i < accepted_requests.size(); i++)
    {
        if(accepted_requests[i].shared_with_username == RIR.shared_with_username && accepted_requests[i].img_ref.image_name == RIR.img_ref.image_name
                && accepted_requests[i].img_ref.owner_username == RIR.img_ref.owner_username)
        {
             agreed_no_views[i] = views_no;

        }
    }


    std::ofstream output;

    output.open(ACCEPTED_REQUESTS_FILENAME, std::ios_base::trunc);

    if(output.is_open())

    {
        for(int i = 0; i< accepted_requests.size(); i++)
        {
            output << accepted_requests.at(i).shared_with_username << " " << accepted_requests.at(i).img_ref.image_name << " " << agreed_no_views.at(i) << " " << accepted_requests.at(i).img_ref.owner_username << std::endl;
        }

        output.close();

    }

    else

    {
        std::cout << "Cannot open file pending requests" << std::endl;
    }

}
