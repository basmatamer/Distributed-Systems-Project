#include "offeredservices.h"
#include "ui_offeredservices.h"
#include "globalvar.h"
#include "requestviews.h"
#include "viewreceivedimage.h"
#include "Configs.h"
#include <QMessageBox>
#include "view_shared_image.h"
#include "image.h"

OfferedServices::OfferedServices(std::vector<ImageReference> ret,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OfferedServices) {
    ui->setupUi(this);

    ui->listWidget->setViewMode(QListWidget::IconMode);
    ui->listWidget->setIconSize(QSize(50,50));
 //   cout<<"retsize:"<<ret.size()<<endl;
    for (unsigned int i=0;i<ret.size();i++) {
    //   ui->listWidget->addItem(QString::fromStdString("Image Name: "+ret[i].image_name+"   Owner: "+ret[i].owner_username));

       std::size_t found = ret[i].image_name.find("SPLITHERE");
       string myimg= ret[i].image_name.substr(found+9,ret[i].image_name.size()-found-9);
       ret[i].image_name=ret[i].image_name.substr(0,found);

       if(!checkDirectoryExists(icons_path+"/"+ret[i].owner_username)) {
           createFolder(icons_path+"/"+ret[i].owner_username);
       }


       image::textToImg(icons_path+"/"+ret[i].owner_username+"/",ret[i].image_name,myimg);

       ui->listWidget->addItem(new QListWidgetItem(QIcon(QString((icons_path+"/"+ret[i].owner_username+"/"+ret[i].image_name).c_str())),QString::fromStdString(ret[i].image_name+"  \n Owner: "+ret[i].owner_username)));

//       cout<<"returned::::"<<ret[i].image_name<<std::endl;
//       cout<<"returned::::"<<myimg<<std::endl;


    //    std::string name_to_show = imported_images[i].substr(imported_images[i].find('_') + 1, imported_images[i].length());

    }
      r=ret;
    if(!ret.empty())
    {
        ui->listWidget->setCurrentItem(ui->listWidget->item(0));
        ui->pushButton_2->setEnabled(true);
    }
}

OfferedServices::~OfferedServices() {
    delete ui;
}

void OfferedServices::on_pushButton_clicked() {
    Search * mysearch = new Search();
              mysearch->show();
    deleteLater();
}

void OfferedServices::on_pushButton_2_clicked() {

    ui->pushButton_2->setEnabled(false);
    int i=ui->listWidget->currentRow();
    if(i >= 0 && (unsigned int)(i) < r.size()) {
        std::string image_to_view_name = "local_" + user_username_string + "_"+ r[i].image_name;// = r[i].owner_username;
//        image_to_view_name += "_";
//        image_to_view_name += user_username_string;
//        image_to_view_name += "_";
//        image_to_view_name += r[i].image_name;

        ifstream test_input;
        std::string path_to_test_with = shared_images_path + r[i].owner_username + "/" + image_to_view_name;
        std::cout << "Attempting to check at " << path_to_test_with << std::endl;
        test_input.open(path_to_test_with.c_str());
        if(test_input.fail()) {
            //open request window
            RequestViews * myrequest = new RequestViews(r,i);
            myrequest->show();
            deleteLater();
        }
        else {
            test_input.close();

            QMessageBox::information(this, "Image Already Owned", "Please View it from your Feed");
            Feed * my_feed = new Feed();
            my_feed->show();
            deleteLater();
            return;

            std::string path_to_view;

            int remaining_views = 0;

            path_to_view = image::receiver_click_extract_image(shared_images_path + r[i].owner_username + "/", "local_" + user_username_string + "_" + r[i].image_name, remaining_views);


            view_shared_image * myview = new view_shared_image(path_to_view,remaining_views,
                                                               r[i].owner_username,
                                                               r[i].image_name, false);
            myview->show();

            deleteLater();
//            ViewReceivedImage * myview = new ViewReceivedImage(r,path_to_view, remaining_views);
//            myview->show();

//            deleteLater();
        }
    }

}

//void OfferedServices::reject()
//{
//    QMessageBox::warning(this, "Stop", "Please logout first.");
//}

