#include "feed.h"
#include "ui_feed.h"
#include "profile.h"
#include "search.h"
#include "Configs.h"
#include <QMessageBox>

Feed::Feed(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Feed) {
    ui->setupUi(this);
    std::ifstream input;
    input.open(PENDING_REQUESTS_FILENAME);
    if(input.peek() != std::ifstream::traits_type::eof())
    {
        ui->label_red_mark->setPixmap(QPixmap(QString::fromStdString(app_path+"notify.png")));
        ui->label_red_mark->setScaledContents(true);
    }
    input.close();
    ui->listWidget_users->setIconSize(QSize(50,50));
    shared_with_users = getNames(shared_images_path);
    for(int i = 0; i < shared_with_users.size(); i++)
    {
        ui->listWidget_users->addItem(QString::fromStdString(shared_with_users.at(i)));
    }
    if(!shared_with_users.empty())
    {
        ui->listWidget_users->setCurrentItem(ui->listWidget_users->item(0));
        ui->pushButton_view->setEnabled(true);
    }
    if(!has_internet)
    {
        ui->notifications_bttn->setEnabled(false);
        ui->search_btn->setEnabled(false);
    }
}

Feed::~Feed() {
    delete ui;
}

void Feed::on_search_btn_clicked() {
    Search * mysearch = new Search();
    mysearch->show();

    deleteLater();
}

void Feed::on_profile_btn_clicked() {
    Profile * myprofile = new Profile();
    myprofile->show();

    deleteLater();
}

void Feed::on_logout_btn_clicked()
{
    attemptToLogout(this);
}

//void Feed::reject()
//{
//    QMessageBox::warning(this, "Stop", "Please logout first.");
//}

void Feed::on_pushButton_view_clicked()
{
    images_user = getNames(shared_images_path+shared_with_users.at(ui->listWidget_users->currentRow()));
    while(ui->listWidget_users_images->count()>0)
    {
      ui->listWidget_users_images->takeItem(0);//handle the item if you don't
                              //have a pointer to it elsewhere
    }
    for(int i = 0; i < images_user.size(); i++)
    {
        ui->listWidget_users_images->addItem(new QListWidgetItem(QIcon(QString((icons_path+"/"+shared_with_users.at(ui->listWidget_users->currentRow())
                                                                                +"/"+images_user.at(i).substr(images_user.at(i).find_last_of('_')+1,
                                                                               images_user.at(i).length())).c_str())),QString::fromStdString
                                                                 (images_user.at(i).substr(images_user.at(i).find_last_of('_')+1,
                                                                                           images_user.at(i).length()))));

    //    ui->listWidget_users_images->addItem(QString::fromStdString(images_user.at(i).substr(images_user.at(i).find_last_of('_')+1, images_user.at(i).length())));
    }
    if(!images_user.empty())
    {
        ui->listWidget_users_images->setCurrentItem(ui->listWidget_users_images->item(0));
        ui->pushButton_view_2->setEnabled(true);
    }
}

void Feed::on_pushButton_view_2_clicked()
{
    std::string path = shared_images_path+shared_with_users.at(ui->listWidget_users->currentRow())
                        +"/";

    int remaining_views = 0;
    std::string path_to_view;
    bool locked = false;

    path_to_view = image::receiver_click_extract_image(path, images_user.at( ui->listWidget_users_images->currentRow()), remaining_views);


//    try {
//        std::string selected_option = images_user.at( ui->listWidget_users_images->currentRow());
//        ImageReferenceMap img_ref_map;
//        img_ref_map.image_name = selected_option.substr(selected_option.find_last_of('_'), selected_option.length());
//        img_ref_map.owner_username = shared_with_users.at(ui->listWidget_users->currentRow());
//        locks_on_images.at(img_ref_map)->lock();
//        locked = true;
//        path_to_view = image::receiver_click_extract_image(path, images_user.at( ui->listWidget_users_images->currentRow()), remaining_views);
//    } catch (...) {
//        path_to_view = image::receiver_click_extract_image(path, images_user.at( ui->listWidget_users_images->currentRow()), remaining_views);
//        std::cout << "Warning .. Unable to find the mutex" << std::endl;
//    }

    view_shared_image * myview = new view_shared_image(path_to_view,remaining_views,
                                                       shared_with_users.at(ui->listWidget_users->currentRow()),
                                                       images_user.at( ui->listWidget_users_images->currentRow()), locked);
    myview->show();

    deleteLater();
}

void Feed::on_notifications_bttn_clicked()
{
    notifications * myNotification = new notifications();
    myNotification->show();
    deleteLater();
}

std::vector<std::string> Feed::getNames(std::string path) {
    const auto& directory_path = std::string(path);
    std::vector<std::string> imagenames = GetDirectoryFiles(path);
//    for(unsigned int i = 0; i < imagenames.size(); i++) {
//        if(imagenames[i].find(".jpg") == std::string::npos || imagenames[i] == DEFAULT_IMAGE_NAME) {
//            imagenames.erase(imagenames.begin() + i);
//            i--;
//        }
//    }
    return imagenames;
}
