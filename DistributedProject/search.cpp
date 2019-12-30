#include "search.h"
#include "ui_search.h"
#include "profile.h"
#include "feed.h"
#include <string>
#include <proxyFunctions.hpp>
#include <globalvar.h>
#include <vector>
#include "offeredservices.h"
#include "Configs.h"
#include <QMessageBox>

#include "profile.h"
#include "ourexception.h"

Search::Search(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Search)
{
    ui->setupUi(this);
    std::ifstream output;

        output.open(PENDING_REQUESTS_FILENAME);

        if(output.peek() != std::ifstream::traits_type::eof())

        {

            ui->label_red_mark->setPixmap(QPixmap(QString::fromStdString(app_path+"notify.png")));

            ui->label_red_mark->setScaledContents(true);

        }

        output.close();
}

Search::~Search()
{
    delete ui;
}

void Search::on_profile_btn_clicked() {
    Profile * myprofile = new Profile();
    myprofile->show();
    deleteLater();
}

void Search::on_feed_btn_clicked() {
    Feed * myfeed = new Feed();
    myfeed->show();
    deleteLater();
}

void Search::on_search_btn_2_clicked() {
    std::string username=ui->editline_username->text().toUtf8().constData();
    if(username.length() < 1 || username.find(" ") != std::string::npos || username.find("_") != std::string::npos) {
        QMessageBox::warning(this,"Invalid name", "No spaces or dashes in the username.\nUsername can't be empty");
        return;
    }
    else
    {
        std::vector<ImageReference> ret;

        ui->editline_username->setEnabled(false);
        ui->feed_btn->setEnabled(false);
        ui->label->setEnabled(false);
        ui->label_2->setEnabled(false);
        ui->label_red_mark->setEnabled(false);
        ui->logout_btn->setEnabled(false);
        ui->notifications_bttn->setEnabled(false);
        ui->profile_btn->setEnabled(false);
        ui->search_btn->setEnabled(false);
        ui->search_btn_2->setEnabled(false);
        ui->view_all->setEnabled(false);

        try {
            proxyFunctions::retrieveServices(myClient, ret);
        } catch(const DirectoryServiceUnreachableException& except) {
            ui->editline_username->setEnabled(true);
            ui->feed_btn->setEnabled(true);
            ui->label->setEnabled(true);
            ui->label_2->setEnabled(true);
            ui->label_red_mark->setEnabled(true);
            ui->logout_btn->setEnabled(true);
            ui->notifications_bttn->setEnabled(true);
            ui->profile_btn->setEnabled(true);
            ui->search_btn->setEnabled(true);
            ui->search_btn_2->setEnabled(true);
            ui->view_all->setEnabled(true);

            QMessageBox::warning(this, "Request not fulfilled.", "Directory Service Unreachable.\nPlease try again later.");



            return;

        } catch(const UnknownUsernameException& except) {
            ui->editline_username->setEnabled(true);
            ui->feed_btn->setEnabled(true);
            ui->label->setEnabled(true);
            ui->label_2->setEnabled(true);
            ui->label_red_mark->setEnabled(true);
            ui->logout_btn->setEnabled(true);
            ui->notifications_bttn->setEnabled(true);
            ui->profile_btn->setEnabled(true);
            ui->search_btn->setEnabled(true);
            ui->search_btn_2->setEnabled(true);
            ui->view_all->setEnabled(true);

            QMessageBox::warning(this, "Request not fulfilled.", "Unknown username used");

            return;

        } catch(const SendingFailedException& except) {
            ui->editline_username->setEnabled(true);
            ui->feed_btn->setEnabled(true);
            ui->label->setEnabled(true);
            ui->label_2->setEnabled(true);
            ui->label_red_mark->setEnabled(true);
            ui->logout_btn->setEnabled(true);
            ui->notifications_bttn->setEnabled(true);
            ui->profile_btn->setEnabled(true);
            ui->search_btn->setEnabled(true);
            ui->search_btn_2->setEnabled(true);
            ui->view_all->setEnabled(true);
            QMessageBox::warning(this, "Connection Error.", "It seems you lost your internet access.\nReconnect and try again later");
            return;

//            has_internet = false;

//            hide();
//            Profile * myProfile = new Profile();
//            myProfile->show();
//            deleteLater();
        }

        std::vector<ImageReference> newret;
        newret.clear();
        for (unsigned int i=0;i<ret.size();i++)
            if(ret[i].owner_username==username)
                newret.push_back(ret[i]);

        OfferedServices * myservices = new OfferedServices(newret);
        myservices->show();
        deleteLater();
    }
}

void Search::on_view_all_clicked() {
    std::vector<ImageReference> ret;

    try {
        ui->editline_username->setEnabled(false);
        ui->feed_btn->setEnabled(false);
        ui->label->setEnabled(false);
        ui->label_2->setEnabled(false);
        ui->label_red_mark->setEnabled(false);
        ui->logout_btn->setEnabled(false);
        ui->notifications_bttn->setEnabled(false);
        ui->profile_btn->setEnabled(false);
        ui->search_btn->setEnabled(false);
        ui->search_btn_2->setEnabled(false);
        ui->view_all->setEnabled(false);

        proxyFunctions::retrieveServices(myClient, ret);
    } catch(const DirectoryServiceUnreachableException& except) {
        ui->editline_username->setEnabled(true);
        ui->feed_btn->setEnabled(true);
        ui->label->setEnabled(true);
        ui->label_2->setEnabled(true);
        ui->label_red_mark->setEnabled(true);
        ui->logout_btn->setEnabled(true);
        ui->notifications_bttn->setEnabled(true);
        ui->profile_btn->setEnabled(true);
        ui->search_btn->setEnabled(true);
        ui->search_btn_2->setEnabled(true);
        ui->view_all->setEnabled(true);

        QMessageBox::warning(this, "Request not fulfilled.", "Directory Service Unreachable.\nPlease try again later.");

        return;

    } catch(const UnknownUsernameException& except) {
        ui->editline_username->setEnabled(true);
        ui->feed_btn->setEnabled(true);
        ui->label->setEnabled(true);
        ui->label_2->setEnabled(true);
        ui->label_red_mark->setEnabled(true);
        ui->logout_btn->setEnabled(true);
        ui->notifications_bttn->setEnabled(true);
        ui->profile_btn->setEnabled(true);
        ui->search_btn->setEnabled(true);
        ui->search_btn_2->setEnabled(true);
        ui->view_all->setEnabled(true);

        QMessageBox::warning(this, "Request not fulfilled.", "Unknown username used");

        return;

    } catch(const SendingFailedException& except) {
        ui->editline_username->setEnabled(true);
        ui->feed_btn->setEnabled(true);
        ui->label->setEnabled(true);
        ui->label_2->setEnabled(true);
        ui->label_red_mark->setEnabled(true);
        ui->logout_btn->setEnabled(true);
        ui->notifications_bttn->setEnabled(true);
        ui->profile_btn->setEnabled(true);
        ui->search_btn->setEnabled(true);
        ui->search_btn_2->setEnabled(true);
        ui->view_all->setEnabled(true);

        QMessageBox::warning(this, "Connection Error.", "It seems you lost your internet access.\nReconnect and try again later");

        return;

//        has_internet = false;

//        hide();
//        Profile * myProfile = new Profile();
//        myProfile->show();
//        deleteLater();
    }

    OfferedServices * myservices = new OfferedServices(ret);
    myservices->show();
    deleteLater();
}

void Search::on_logout_btn_clicked()
{
    attemptToLogout(this);
}

//void Search::reject()
//{
//    QMessageBox::warning(this, "Stop", "Please logout first.");
//}


void Search::on_notifications_bttn_clicked()
{
    notifications * myNotifications = new notifications();
    myNotifications->show();
    deleteLater();
}
