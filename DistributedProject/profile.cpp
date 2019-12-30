#include "profile.h"
#include "ui_profile.h"
#include "feed.h"
#include "search.h"
#include "view_image.h"
#include "enter_username.h"
#include "ui_profile.h"
#include "view_statistics.h"
#include "image.h"

#include <QFileDialog>
#include <QListWidget>
#include <QMessageBox>

#include "InterfaceDefintion.h"
#include "proxyFunctions.hpp"
#include "globalvar.h"
#include "Configs.h"
#include "image.h"
#include "notifications.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <memory>
#include <cstring>

#include "ourexception.h"

Profile::Profile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Profile) {
    ui->setupUi(this);
    imported_images = getImageNames();
    std::ifstream output;

        output.open(PENDING_REQUESTS_FILENAME);

        if(output.peek() != std::ifstream::traits_type::eof())

        {

            ui->label_red_mark->setPixmap(QPixmap(QString::fromStdString(app_path+"notify.png")));

            ui->label_red_mark->setScaledContents(true);

        }

        ui->listWidget_profile->setViewMode(QListWidget::IconMode);
        ui->listWidget_profile->setIconSize(QSize(50,50));

        output.close();

    for(unsigned int i = 0; i < imported_images.size(); i++) {
        std::string name_to_show = imported_images[i].substr(imported_images[i].find('_') + 1, imported_images[i].length());
      //  ui->listWidget_profile->addItem(QString::fromStdString(name_to_show));

        ui->listWidget_profile->addItem(new QListWidgetItem(QIcon(QString((icons_path+name_to_show).c_str())),QString(name_to_show.c_str())));

    }

    if(!imported_images.empty())
    {
        ui->listWidget_profile->setCurrentItem(ui->listWidget_profile->item(0));
        ui->pushButton_view_image->setEnabled(true);
    }

    if(!has_internet)
    {
        ui->notifications_bttn->setEnabled(false);
        ui->search_btn->setEnabled(false);
        ui->pushButton_import->setEnabled(false);
    }
}

Profile::~Profile() {
    delete ui;
}

void Profile::on_feed_btn_clicked() {
    Feed * myfeed = new Feed();
    myfeed->show();
    deleteLater();
}

void Profile::on_search_btn_clicked() {
    Search * mysearch = new Search();
    mysearch->show();
    deleteLater();
}

void Profile::on_pushButton_import_clicked() {
    QString filename = QFileDialog::getOpenFileName(this ,tr("Import Image"), "/home", "Images (*.jpg)");
    std::string selected_file_path = filename.toStdString();
    if(selected_file_path.length() < 5) {
        return;
    }
    std::string picture_name = getFileName(selected_file_path);
    std::string picture_path = getPathOnly(selected_file_path);

    if(picture_name.length() < 5) {
        QMessageBox::warning(this,"Invalid Filename", QString::fromStdString(picture_name + " is not a valid file name"));
        return;
    }
    if(picture_name.find('_') != std::string::npos) {
        QMessageBox::warning(this,"Invalid Filename", "Picture name can't include _");
        return;
    }
    if(picture_name.find(' ') != std::string::npos) {
        QMessageBox::warning(this,"Invalid Filename", "Picture name can't include a space");
        return;
    }
    if(picture_name.find('.') != picture_name.length() - 4) {
        QMessageBox::warning(this,"Invalid Filename", "Picture name can't include a dot");
        return;
    }
    uint32_t image_file_size = filesize(selected_file_path.c_str());
    if(image_file_size < 1) {
        QMessageBox::warning(this,"Invalid File", "The selected image file is Empty!");
        return;
    }
    if(image_file_size > MAX_FILE_SIZE) {
        QMessageBox::warning(this,"Invalid File",  QString::fromStdString("The selected image file is too big!\nMaximum supported file size is " + std::to_string(MAX_FILE_SIZE / 1024) + " KB"));
        return;
    }
    if(picture_name == DEFAULT_IMAGE_NAME) {
        QMessageBox::warning(this,"Invalid Filename", QString::fromStdString(std::string(DEFAULT_IMAGE_NAME) + " is a reserved name.\nPlease rename this image first."));
        return;
    }
    std::string local_picture_name = "local_" + picture_name;

    for(unsigned int i = 0; i < imported_images.size(); i++) {
        if(local_picture_name == imported_images[i]) {
            QMessageBox::warning(this,"Invalid Filename", "You already imported an image with this exact name before");
            return;
        }
    }

    std::ifstream test_original_image;
    test_original_image.open(picture_path + picture_name);
    if(test_original_image.is_open()) {
        test_original_image.close();
    } else {
        QMessageBox::warning(this,"Error", "Unable to attempt importing image");
        return;
    }

    ui->pushButton_import->setEnabled(false);
    ui->pushButton_statistics->setEnabled(false);
    ui->pushButton_view_image->setEnabled(false);
    ui->feed_btn->setEnabled(false);
    ui->logout_btn->setEnabled(false);
    ui->profile_btn->setEnabled(false);
    ui->notifications_bttn->setEnabled(false);
    ui->feed_btn->setEnabled(false);
    ui->search_btn->setEnabled(false);

    QImage img=QImage((picture_path + picture_name).c_str());
    QImage img2 = img.scaled(50, 50, Qt::KeepAspectRatio);
    img2.save(QString::fromStdString(icons_path+picture_name));

    std::string newIcon = image::imgToText(icons_path,picture_name);

    try {
        if(proxyFunctions::announceImage(myClient, picture_name+"SPLITHERE"+newIcon) == false)
        {
            ui->pushButton_import->setEnabled(true);
            ui->pushButton_statistics->setEnabled(true);
            ui->pushButton_view_image->setEnabled(true);
            ui->feed_btn->setEnabled(true);
            ui->logout_btn->setEnabled(true);
            ui->profile_btn->setEnabled(true);
            ui->notifications_bttn->setEnabled(true);
            ui->feed_btn->setEnabled(true);
            ui->search_btn->setEnabled(true);
            QMessageBox::warning(this,"Announcement failed", "Directory service is unreachable");
            return;
        }
    } catch(const DirectoryServiceUnreachableException& except) {
        QMessageBox::warning(this, "Request not fulfilled.", "Directory Service Unreachable.\nPlease try again later.");

        ui->pushButton_import->setEnabled(true);
        ui->pushButton_statistics->setEnabled(true);
        ui->pushButton_view_image->setEnabled(true);
        ui->feed_btn->setEnabled(true);
        ui->logout_btn->setEnabled(true);
        ui->profile_btn->setEnabled(true);
        ui->notifications_bttn->setEnabled(true);
        ui->feed_btn->setEnabled(true);
        ui->search_btn->setEnabled(true);

        return;

    } catch(const UnknownUsernameException& except) {
        QMessageBox::warning(this, "Request not fulfilled.", "Unknown username used");

        ui->pushButton_import->setEnabled(true);
        ui->pushButton_statistics->setEnabled(true);
        ui->pushButton_view_image->setEnabled(true);
        ui->feed_btn->setEnabled(true);
        ui->logout_btn->setEnabled(true);
        ui->profile_btn->setEnabled(true);
        ui->notifications_bttn->setEnabled(true);
        ui->feed_btn->setEnabled(true);
        ui->search_btn->setEnabled(true);

        return;

    } catch(const SendingFailedException& except) {
        QMessageBox::warning(this, "Connection Error.", "It seems you lost your internet access.\nReconnect and try again later");

        ui->pushButton_import->setEnabled(true);
        ui->pushButton_statistics->setEnabled(true);
        ui->pushButton_view_image->setEnabled(true);
        ui->feed_btn->setEnabled(true);
        ui->logout_btn->setEnabled(true);
        ui->profile_btn->setEnabled(true);
        ui->notifications_bttn->setEnabled(true);
        ui->feed_btn->setEnabled(true);
        ui->search_btn->setEnabled(true);

//        has_internet = false;

//        ui->notifications_bttn->setEnabled(false);
//        ui->search_btn->setEnabled(false);
//        ui->pushButton_import->setEnabled(false);

        return;
    }



    ui->pushButton_import->setEnabled(true);
    ui->pushButton_statistics->setEnabled(true);
    ui->pushButton_view_image->setEnabled(true);
    ui->feed_btn->setEnabled(true);
    ui->logout_btn->setEnabled(true);
    ui->profile_btn->setEnabled(true);
    ui->notifications_bttn->setEnabled(true);
    ui->feed_btn->setEnabled(true);
    ui->search_btn->setEnabled(true);

    QFile::copy(QString::fromStdString(picture_path + picture_name), QString::fromStdString(owned_images_path + picture_name));

    image::copyImg(owned_images_path, DEFAULT_IMAGE_NAME, local_picture_name);
    image::embedStegImg(owned_images_path,local_picture_name , picture_name);
    std::remove((owned_images_path + picture_name).c_str());

    std::ifstream test_new_image;
    test_new_image.open(owned_images_path+local_picture_name);
    if(test_new_image.is_open()) {
        test_new_image.close();
        imported_images.push_back(local_picture_name);
     //   ui->listWidget_profile->addItem(QString::fromStdString(picture_name));
        //set icon

        ui->listWidget_profile->addItem(new QListWidgetItem(QIcon(QString((icons_path+picture_name).c_str())),QString(picture_name.c_str())));



        QMessageBox::information(this,"Success", QString::fromStdString(picture_name + " imported succesfully.\nYou may now share it with others!"));

    } else {
        QMessageBox::warning(this,"Error", "Failed to import image");
        return;
    }

    if(!imported_images.empty())
    {
        ui->listWidget_profile->setCurrentItem(ui->listWidget_profile->item(0));
        ui->pushButton_view_image->setEnabled(true);
    }
}

std::vector<std::string> Profile::getImageNames() {
    const auto& directory_path = std::string(owned_images_path);
    std::vector<std::string> imagenames = GetDirectoryFiles(owned_images_path);
    for(unsigned int i = 0; i < imagenames.size(); i++) {
        if(imagenames[i].find(".jpg") == std::string::npos || imagenames[i] == DEFAULT_IMAGE_NAME) {
            imagenames.erase(imagenames.begin() + i);
            i--;
        }
    }
    return imagenames;
}

//void profile::on_pushButton_statistics_clicked()
//{

//}

void Profile::on_pushButton_view_image_clicked()
{
    int selected_index = ui->listWidget_profile->currentRow();
    if(selected_index >= 0 && (uint32_t)(selected_index) < imported_images.size()) {
        view_image * myViewImage = new view_image(imported_images[selected_index]);
        myViewImage->show();
        deleteLater();
    }
}

void Profile::on_logout_btn_clicked()
{
   attemptToLogout(this);
}

//void Profile::reject()
//{
//    QMessageBox::warning(this, "Stop", "Please logout first.");
//}

void Profile::on_notifications_bttn_clicked()
{
    notifications * myNotifications = new notifications();
    myNotifications->show();
    deleteLater();
}

void Profile::on_pushButton_statistics_clicked()
{
    view_statistics * myView = new view_statistics();
    myView->show();
    deleteLater();
}
