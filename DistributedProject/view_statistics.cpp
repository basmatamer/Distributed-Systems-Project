#include "view_statistics.h"
#include "ui_view_statistics.h"

view_statistics::view_statistics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::view_statistics)
{
    ui->setupUi(this);

    any_log->robot_read_statistics(statisticsMap);
    std::cout << "map size is: "<< statisticsMap.size() << std::endl;
    if(!statisticsMap.empty())
    {

        //populate list and use map
        ui->label_total_img_3->setText(QString::fromStdString(std::to_string(statisticsMap.size())));
        for(std::map<std::string,std::vector<ImageUsersViews>>::iterator it = statisticsMap.begin(); it != statisticsMap.end(); ++it)
        {
          ui->listWidget_images->addItem(QString::fromStdString(it->first));
        }
        ui->listWidget_images->setCurrentItem(ui->listWidget_images->item(0));
        ui->pushButton_view->setEnabled(true);
    }
}

view_statistics::~view_statistics()
{
    delete ui;
}

void view_statistics::on_pushButton_view_clicked()
{
    std::cout <<"current image: "<< ui->listWidget_images->currentItem()->text().toUtf8().constData() << std::endl;
    while(ui->listWidget_users_image->count()>0)
    {
      ui->listWidget_users_image->takeItem(0);//handle the item if you don't
                              //have a pointer to it elsewhere
    }
    std::vector<ImageUsersViews> temp =  statisticsMap.at(ui->listWidget_images->currentItem()->text().toUtf8().constData());
    for(int i = 0; i < temp.size(); i++)
    {
        ui->listWidget_users_image->addItem(QString::fromStdString(temp.at(i).shared_with_username + "\t"
                                                                   + std::to_string(temp.at(i).actual_views)
                                                                   + " views"));
    }
}

void view_statistics::on_pushButton_back_clicked()
{
    Profile * myView = new Profile();
    myView->show();
    deleteLater();
}
