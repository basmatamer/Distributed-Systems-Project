#include "handle_notifications.h"
#include "ui_handle_notifications.h"
#include <QMessageBox>

#include "ourexception.h"

#include "profile.h"
#include "Configs.h"

handle_notifications::handle_notifications(std::string msg,bool views_indicator, std::vector<RemoteImageReference> pending_requests,
                                           std::vector<uint32_t> no_views,unsigned int index, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::handle_notifications)
{
    ui->setupUi(this);
    this->pending_requests = pending_requests;
    this->no_views = no_views;
    position = index;
    this->views_indicator = views_indicator;
    ui->label_notifications->setText(QString::fromStdString(msg));
}

handle_notifications::~handle_notifications()
{
    delete ui;
}

void handle_notifications::on_pushButton_accept_clicked()
{
    ui->pushButton_accept->setEnabled(false);
    ui->pushButton_deny->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->lineEdit_views->setEnabled(false);
    try {
        std::string actual_views_no = ui->lineEdit_views->text().toUtf8().constData();
        if(std::stoi(actual_views_no) > 0) {
            try {

                if(views_indicator)
                {
                    PendingQuotaChange temp;
                    temp.remote_img_ref = pending_requests.at(position);
                    temp.new_quota = std::stoi(actual_views_no) + no_views[position];


                    if(proxyFunctions::changeQouta(myClient, temp))
                    {
                        //delete from file
                        updatePendingRequestsLog();
                        QMessageBox::information(this, "Success", "Additional views succesfully granted");
                        notifications * myNotif = new notifications();
                        myNotif->show();
                        deleteLater();
                    }
                        else
                    {
                        std::cout << "Change quota failed" << std::endl;
                        QMessageBox::warning(this, "Error", "Request not fulfilled.\nPlease try again later.");

                        notifications * myNotif = new notifications();
                                    myNotif->show();
                                    deleteLater();
                    }
                }
                else
                {
                    if(proxyFunctions::grantImage(myClient, this->pending_requests.at(position), std::stoi(actual_views_no)))
                    {
                        //delete from file
                        updatePendingRequestsLog();
                        QMessageBox::information(this, "Success", "This image was succesfully granted");
                        notifications * myNotif = new notifications();
                        myNotif->show();
                        deleteLater();
                    }
                        else
                    {
                        std::cout << "grant image failed" << std::endl;
                        QMessageBox::warning(this, "Error", "Request not fulfilled.\nPlease try again later.");

                        notifications * myNotif = new notifications();
                                    myNotif->show();
                                    deleteLater();
                    }
                }

            } catch(const DirectoryServiceUnreachableException& except) {
                QMessageBox::warning(this, "Request not fulfilled.", "Directory Service Unreachable.\nPlease try again later.");

                notifications * myNotif = new notifications();
                myNotif->show();
                deleteLater();

            } catch(const UnknownUsernameException& except) {
                QMessageBox::warning(this, "Request not fulfilled.", "Unknown username used");

                notifications * myNotif = new notifications();
                myNotif->show();
                deleteLater();

            } catch(const SendingFailedException& except) {
                QMessageBox::warning(this, "Connection Error.", "It seems you lost your internet access.\nReconnect and try again later");

                notifications * myNotif = new notifications();
                myNotif->show();
                deleteLater();


                // has_internet = false;

//                Profile * pf = new Profile();
//                pf->show();
//                deleteLater();
            }


        } else {
            QMessageBox::warning(this, "Invalid Input", "You must give positive views!");
            ui->pushButton_accept->setEnabled(true);
            ui->pushButton_deny->setEnabled(true);
            ui->pushButton_3->setEnabled(true);
            ui->lineEdit_views->setEnabled(true);
            return;
        }
    }
    catch (...) {
        QMessageBox::warning(this, "Invalid Input", "This was not an integer!");
        ui->pushButton_accept->setEnabled(true);
        ui->pushButton_deny->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->lineEdit_views->setEnabled(true);
        return;
    }
}

void handle_notifications::on_pushButton_deny_clicked()
{
   ui->pushButton_deny->setEnabled(false);
   ui->pushButton_accept->setEnabled(false);
   updatePendingRequestsLog();
   notifications * myNotif = new notifications();
   myNotif->show();
   deleteLater();
}

void handle_notifications::on_pushButton_3_clicked()
{
    notifications * myNotif = new notifications();
    myNotif->show();
    deleteLater();
}

void handle_notifications::updatePendingRequestsLog()
{
    pending_requests.erase(pending_requests.begin()+position);
    no_views.erase(no_views.begin()+position);
    any_log->rewrite_pending_requests(pending_requests, no_views);
}
