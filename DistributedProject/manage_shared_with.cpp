#include "manage_shared_with.h"
#include "ui_manage_shared_with.h"
#include <QMessageBox>
#include "profile.h"
#include "image.h"

#include "ourexception.h"

manage_shared_with::manage_shared_with(std::string image_name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::manage_shared_with)
{
    ui->setupUi(this);

    this->image_name = image_name;

    any_log->read_accepted_requests(accepted_requests, no_views);
    any_log->robot_read_statistics(statisticsMap);

    for(int i = 0; i < accepted_requests.size(); i++)
    {
        std::cout<<"images versus each other "+ accepted_requests.at(i).img_ref.image_name << " vs "<<image_name.substr(image_name.find_last_of('_')+1, image_name.length())<<std::endl;
      int current_views;
        for(int j = 0; j < statisticsMap[accepted_requests.at(i).img_ref.image_name].size(); j++)
      {
        std::cout << "Inside loooooooooooop" << std::endl;
        std::cout << statisticsMap[accepted_requests.at(i).img_ref.image_name][j].shared_with_username << statisticsMap[accepted_requests.at(i).img_ref.image_name][j].actual_views<< std::endl;
          if(accepted_requests[i].shared_with_username == statisticsMap[accepted_requests.at(i).img_ref.image_name][j].shared_with_username)
            current_views = statisticsMap[accepted_requests.at(i).img_ref.image_name][j].actual_views;
      }

       if(accepted_requests.at(i).img_ref.image_name == image_name.substr(image_name.find_last_of('_')+1, image_name.length()))
       {
           ui->listWidget_shared_with->addItem(QString::fromStdString(accepted_requests.at(i).shared_with_username
                                                                    + "\t"
                                                                    + std::to_string(no_views.at(i))
                                                                    + "\t"
                                                                    + std::to_string(current_views))
                                                                    );
           indices.push_back(i);
       }
    }
    if(!accepted_requests.empty())
    {
        ui->listWidget_shared_with->setCurrentItem(ui->listWidget_shared_with->item(0));
        ui->pushButton_update_2->setEnabled(true);
    }

    if(!has_internet)
    {
        ui->pushButton_update_2->setEnabled(false);
    }
}

manage_shared_with::~manage_shared_with()
{
    delete ui;
}

void manage_shared_with::on_pushButton_back_clicked()
{
    Profile * myProfile = new Profile();
    myProfile->show();
    deleteLater();
}

void manage_shared_with::on_pushButton_update_2_clicked()
{
    ui->pushButton_update_2->setEnabled(false);
    ui->pushButton_back->setEnabled(false);
    ui->lineEdit_views_2->setEnabled(false);
    try {
        uint32_t quota = std::stoi(ui->lineEdit_views_2->text().toUtf8().constData());

        if(quota > 0) {
            PendingQuotaChange change;
            change.remote_img_ref = accepted_requests.at(indices.at(ui->listWidget_shared_with->currentRow()));
            change.new_quota = quota;

            bool result;

            try {
                result = proxyFunctions::changeQouta(myClient, change);

            } catch(const DirectoryServiceUnreachableException& except) {
                QMessageBox::warning(this, "Request not fulfilled.", "Directory Service Unreachable.\nPlease try again later.");

                hide();
                Profile * myProfile = new Profile();
                myProfile->show();
                deleteLater();

            } catch(const UnknownUsernameException& except) {
                QMessageBox::warning(this, "Request not fulfilled.", "Unknown username used");

                hide();
                Profile * myProfile = new Profile();
                myProfile->show();
                deleteLater();

            } catch(const SendingFailedException& except) {
                QMessageBox::warning(this, "Connection Error.", "It seems you lost your internet access.\nReconnect and try again later");


                hide();
                Profile * myProfile = new Profile();
                myProfile->show();
                deleteLater();
            }


            if(result) {
                QMessageBox::information(this, "Success", "This image views were succesfully changed");
                hide();
                Profile * myProfile = new Profile();
                myProfile->show();
                deleteLater();
            } else {
                QMessageBox::warning(this, "Error", "Your request not fulfilled.\nPlease try again later.");
                hide();
                Profile * myProfile = new Profile();
                myProfile->show();
                deleteLater();
            }
        } else {
            QMessageBox::warning(this, "Invalid Input", "You must enter a positive quota!");
            ui->pushButton_update_2->setEnabled(true);
            ui->pushButton_back->setEnabled(true);
            ui->lineEdit_views_2->setEnabled(true);
            return;
        }
    }
    catch (...) {
        QMessageBox::warning(this, "Invalid Input", "This was not an integer!");
        ui->pushButton_update_2->setEnabled(true);
        ui->pushButton_back->setEnabled(true);
        ui->lineEdit_views_2->setEnabled(true);
        return;
    }
}
