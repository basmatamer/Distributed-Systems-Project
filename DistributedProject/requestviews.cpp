#include "requestviews.h"
#include "ui_requestviews.h"
#include "offeredservices.h"
#include <QMessageBox>
#include "proxyFunctions.hpp"
#include "globalvar.h"

#include "profile.h"
#include "ourexception.h"

RequestViews::RequestViews(std::vector<ImageReference> ret,int i,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RequestViews)
{
    r=ret;
    index = i;
    ui->setupUi(this);
}

RequestViews::~RequestViews()
{
    delete ui;
}

void RequestViews::on_pushButton_clicked()
{
    OfferedServices * myservices = new OfferedServices(r);
    myservices->show();
    deleteLater();
}

//void RequestViews::reject()
//{
//    QMessageBox::warning(this, "Stop", "Please logout first.");
//}


void RequestViews::on_request_button_clicked()
{
    ui->request_button->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->quota_editText->setEnabled(false);
    try {
        int quota = std::stoi(ui->quota_editText->toPlainText().toStdString());
        if(quota > 0) {
            RemoteImageReference RIR;
            bool result;

            try {
                result = proxyFunctions::requestService_client(myClient, r[index], (uint32_t)(quota), RIR);

            } catch(const DirectoryServiceUnreachableException& except) {
                QMessageBox::warning(this, "Request not fulfilled.", "Directory Service Unreachable.\nPlease try again later.");

                Search * search = new Search();
                search->show();
                deleteLater();

            } catch(const UnknownUsernameException& except) {
                QMessageBox::warning(this, "Request not fulfilled.", "Unknown username used");

                Search * search = new Search();
                search->show();
                deleteLater();

            } catch(const SendingFailedException& except) {
                QMessageBox::warning(this, "Connection Error.", "It seems you lost your internet access.\nReconnect and try again later");

                Search * search = new Search();
                search->show();
                deleteLater();

//                has_internet = false;

//                hide();
//                Profile * myProfile = new Profile();
//                myProfile->show();
//                deleteLater();
            }

            if(result) {
                QMessageBox::information(this, "Success", "The Request has been successfully sent");
                Search * search = new Search();
                search->show();
                deleteLater();

                // TODO
                // reply with acknowledgment to sender
            } else {
                QMessageBox::warning(this, "Error", "Request not fulfilled.\nPlease try again later.");
                Search * search = new Search();
                search->show();
                deleteLater();
            }
        } else {
            QMessageBox::warning(this, "Invalid Input", "You must request a positive quota!");
            ui->request_button->setEnabled(true);
            ui->pushButton->setEnabled(true);
            ui->quota_editText->setEnabled(true);
            return;
        }
    }
    catch (...) {
        QMessageBox::warning(this, "Invalid Input", "This was not an integer!");
        ui->request_button->setEnabled(true);
        ui->pushButton->setEnabled(true);
        ui->quota_editText->setEnabled(true);
        return;
    }
}
