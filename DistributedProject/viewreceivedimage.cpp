#include "viewreceivedimage.h"
#include "ui_viewreceivedimage.h"

#include <QMessageBox>


ViewReceivedImage::ViewReceivedImage(std::vector<ImageReference> ret, string path, int remaining_views, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewReceivedImage) {
    r=ret;
    ui->setupUi(this);

    std::cout << "Viewing image at " << path << std::endl;
    std::cout << "We have " << remaining_views << " remaining views" << std::endl;
    ui->views_label->setText(QString::fromStdString("You have " + std::to_string(remaining_views) + " remaining views"));

    ui->myImg->setPixmap(QString::fromStdString(path));
    ui->myImg->setScaledContents(true);
    if(remaining_views > 0) {
        std::cout << "Deleting extracted image file" << std::endl;
        std::remove(path.c_str());
    }
}

ViewReceivedImage::~ViewReceivedImage() {
    delete ui;
}

void ViewReceivedImage::on_pushButton_clicked() {
    OfferedServices * myservices = new OfferedServices(r);
    myservices->show();
    deleteLater();
}

//void ViewReceivedImage::reject()
//{
//    QMessageBox::warning(this, "Stop", "Please logout first.");
//}

