#include "notifications.h"
#include "ui_notifications.h"
#include <fstream>
#include "handle_notifications.h"
#include "search.h"
#include "profile.h"
#include "feed.h"

notifications::notifications(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::notifications)
{
    ui->setupUi(this);
    std::ifstream input;
    input.open(PENDING_REQUESTS_FILENAME);
    if(input.is_open())
    {
        if(input.peek() != std::ifstream::traits_type::eof())
        {
            ui->label_red_mark->setPixmap(QPixmap(QString::fromStdString(app_path+"notify.png")));
            ui->label_red_mark->setScaledContents(true);
        }
    }


    any_log->read_pending_requests(pending_requests, no_views);

    any_log->read_accepted_requests(accepted_requests, agreed_no_views);

    std::cout << pending_requests.size() << " " << accepted_requests.size() << std::endl;

    for(int i = 0; i < pending_requests.size(); i++)
    {
        bool flag = false;

        for(int j = 0; j < accepted_requests.size(); j++)
        {
            if(pending_requests[i].img_ref.image_name == accepted_requests[j].img_ref.image_name &&
                    pending_requests[i].img_ref.owner_username == accepted_requests[j].img_ref.owner_username &&
                    pending_requests[i].shared_with_username == accepted_requests[j].shared_with_username)
            {
               flag = true;
               indices_more_views.push_back(i);
               indices_agreed_views.push_back(j);
            }
        }

        if(flag)
        {
            this->msg = pending_requests.at(i).shared_with_username
                   + " has requested "
                   + std::to_string(no_views.at(i))
                   + " additional views"
                   + " for "
                   + pending_requests.at(i).img_ref.image_name;
        }
        else
        {
            this->msg = pending_requests.at(i).shared_with_username
                   + " has requested "
                   + pending_requests.at(i).img_ref.image_name
                   + " for "
                   + std::to_string(no_views.at(i))
                   + " views";
        }
        ui->listWidget_notifications->addItem(QString::fromStdString(msg));
    }


    if(!pending_requests.empty())
    {
        ui->listWidget_notifications->setCurrentItem(ui->listWidget_notifications->item(0));
        ui->pushButton_handle->setEnabled(true);
    }
}

notifications::~notifications()
{
    delete ui;
}

void notifications::on_pushButton_handle_clicked()
{
    ui->pushButton_handle->setEnabled(false);
    bool flag_indices = false;

    for(int i = 0; i < indices_more_views.size(); i++)
    {
        if(ui->listWidget_notifications->currentRow() == indices_more_views[i])
        {
            flag_indices = true;
            no_views[indices_more_views[i]] = agreed_no_views[indices_agreed_views[i]];
        }
    }

    msg = ui->listWidget_notifications->currentItem()->text().toStdString();


    handle_notifications * myHandleNotifications = new handle_notifications(msg,flag_indices, pending_requests, no_views,
                                                                                ui->listWidget_notifications->currentRow());

    myHandleNotifications->show();
    deleteLater();

}

void notifications::on_profile_btn_clicked()
{
    Profile * myProfile = new Profile();
    myProfile->show();
    deleteLater();
}

void notifications::on_feed_btn_clicked()
{
    Feed * myfeed = new Feed();
    myfeed->show();
    deleteLater();
}

void notifications::on_notifications_bttn_clicked()
{
    Search * mysearch = new Search();
    mysearch->show();
    deleteLater();
}

void notifications::on_logout_btn_clicked()
{
     attemptToLogout(this);
}
