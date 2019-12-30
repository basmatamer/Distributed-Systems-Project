#include "view_shared_image.h"
#include "ui_view_shared_image.h"
#include "Configs.h"

view_shared_image::view_shared_image(std::string path, int remaining_views, std::string shared_username, std::string image_name, bool locked,
                                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::view_shared_image)
{
    ui->setupUi(this);
    ui->myImg->setPixmap(QPixmap(QString::fromStdString(path)));
    ui->myImg->setScaledContents(true);
    this->shared_username = shared_username;
    this->image_name = image_name;
    int views;
    if(remaining_views > 0)
    {
        views = remaining_views - 1;
    }
    ui->label_views_left->setText(QString::fromStdString(std::to_string(views)));

    if(remaining_views > 0) {
        std::cout << "Deleting extracted image file" << std::endl;
        std::remove(path.c_str());
    }

//    if(locked) {
//        try {
//            ImageReferenceMap img_ref_map;
//            img_ref_map.image_name = image_name.substr(image_name.find_last_of('_'), image_name.length());
//            img_ref_map.owner_username = shared_username;
//            locks_on_images.at(img_ref_map)->unlock();
//        } catch(...) {
//            std::cout << "Warning .. Unable to find mutex" << std::endl;
//        }
//    }

    if(!has_internet)
    {
        ui->pushButton_->setEnabled(false);
    }
}

view_shared_image::~view_shared_image()
{
    delete ui;
}

void view_shared_image::on_pushButton__clicked()
{
   ImageReference temp;
   temp.image_name = image_name.substr(image_name.find_last_of('_')+1, image_name.length());
   temp.owner_username = shared_username;
   std::vector<ImageReference> ret;
   ret.push_back(temp);
   request_more_views * myView = new request_more_views(ret, 0);
   myView->show();
   deleteLater();
}

void view_shared_image::on_pushButton_back_clicked()
{
    Feed * myFeed = new Feed();
    myFeed->show();
    deleteLater();
}
