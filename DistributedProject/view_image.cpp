#include "view_image.h"
#include "ui_view_image.h"
#include "QMessageBox"

#include "profile.h"
#include "Configs.h"
#include "proxyFunctions.hpp"
#include "globalvar.h"
#include "image.h"
#include "manage_shared_with.h"

#include <string>


view_image::view_image(const std::string& image_name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::view_image)
{
    this_image_name = image_name;
    ui->setupUi(this);
    std::string path_to_view = image::owner_click_extract_image(owned_images_path, image_name);
    ui->label_pic->setPixmap(QString::fromStdString(path_to_view));
    // TODO
    // delete the extracted image in path_to_view

    std::remove(path_to_view.c_str());

    ui->label_pic->setScaledContents(true);
}

view_image::~view_image()
{
    delete ui;
}

void view_image::on_pushButton_back_clicked()
{
    Profile * profile_view = new Profile();
    profile_view->show();
    deleteLater();
}

//void view_image::reject()
//{
//    QMessageBox::warning(this, "Stop", "Please logout first.");
//}

void view_image::on_pushButton_offer_to_2_clicked()
{
    //shared with
    manage_shared_with * myView = new manage_shared_with(this_image_name);
    myView->show();
    deleteLater();
}
