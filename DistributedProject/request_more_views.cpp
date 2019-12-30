#include "request_more_views.h"
#include "ui_request_more_views.h"
#include "feed.h"

#include "profile.h"
#include "ourexception.h"

request_more_views::request_more_views(std::vector<ImageReference> ret,int i,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::request_more_views)
{
    ui->setupUi(this);
    this->ret = ret;
    this->position = i;
}

request_more_views::~request_more_views()
{
    delete ui;
}

void request_more_views::on_pushButton_clicked()
{
    Feed * myFeed = new Feed();
    myFeed->show();
    deleteLater();

}

void request_more_views::on_request_button_clicked()
{
    ui->request_button->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->lineEdit_quota->setEnabled(false);
    try {
            int quota = std::stoi(ui->lineEdit_quota->text().toUtf8().constData());
            if(quota > 0) {
                RemoteImageReference RIR;

                bool result;

                try {
                    result = proxyFunctions::request_more_views(myClient, ret[position], (uint32_t)(quota));
                } catch(const DirectoryServiceUnreachableException& except) {
                    QMessageBox::warning(this, "Request not fulfilled.", "Directory Service Unreachable.\nPlease try again later.");

                    Feed * myFeed = new Feed();
                    myFeed->show();
                    deleteLater();

                } catch(const UnknownUsernameException& except) {
                    QMessageBox::warning(this, "Request not fulfilled.", "Unknown username used");

                    Feed * myFeed = new Feed();
                    myFeed->show();
                    deleteLater();

                } catch(const SendingFailedException& except) {
                    QMessageBox::warning(this, "Connection Error.", "It seems you lost your internet access.\nReconnect and try again later");

                    Feed * myFeed = new Feed();
                    myFeed->show();
                    deleteLater();

//                    has_internet = false;

//                    hide();
//                    Profile * myProfile = new Profile();
//                    myProfile->show();
//                    deleteLater();
                }



                if(result) {
                    QMessageBox::information(this, "Success", "This image additional views were succesfully requested");
                    Feed * myFeed = new Feed();
                    myFeed->show();
                    deleteLater();

                    // TODO
                    // reply with acknowledgment to sender
                } else {
                    QMessageBox::warning(this, "Error", "Request not fulfilled.\nPlease try again later.");
                    Feed * myFeed = new Feed();
                    myFeed->show();
                    deleteLater();

                }
            } else {
                QMessageBox::warning(this, "Invalid Input", "You must request a positive quota!");
                ui->request_button->setEnabled(true);
                ui->pushButton->setEnabled(true);
                ui->lineEdit_quota->setEnabled(true);
                return;
            }
        }
        catch (...) {
            QMessageBox::warning(this, "Invalid Input", "This was not an integer!");
            ui->request_button->setEnabled(true);
            ui->pushButton->setEnabled(true);
            ui->lineEdit_quota->setEnabled(true);
            return;
        }
}
