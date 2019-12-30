#ifndef LOGS_H
#define LOGS_H


#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include "InterfaceDefintion.h"
#include <mutex>

class logs
{
private:
    std::vector<std::string> get_keys_of_map(std::map< std::string , std::vector<ImageUsersViews> > m);
    std::mutex mutex_statistics;
    std::mutex mutex_pending;
    std::mutex mutex_accepted;

public:
    logs();

    void robot_write_statistics (std::map<std::string , std::vector<ImageUsersViews> > new_robot_results);
    bool robot_read_statistics(std::map<std::string , std::vector<ImageUsersViews> >&);

    //pending requests
    void write_pending_requests(RemoteImageReference RIR, uint32_t views_no);
    void read_pending_requests(std::vector<RemoteImageReference>& pending_requests, std::vector<uint32_t>& no_views);
    void rewrite_pending_requests(std::vector<RemoteImageReference> pending_requests, std::vector<uint32_t> no_views);

    //accepted requests
    void write_accepted_requests(RemoteImageReference RIR, uint32_t views_no);
    void read_accepted_requests(std::vector<RemoteImageReference>& accepted_requests, std::vector<uint32_t>& no_views);
    void update_accepted_requests(RemoteImageReference RIR, uint32_t views_no);



};

#endif // LOGS_H
