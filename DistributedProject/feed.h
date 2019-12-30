#ifndef FEED_H
#define FEED_H

#include <QDialog>
#include "image.h"
#include "search.h"
#include "notifications.h"
#include "InterfaceDefintion.h"
#include <vector>
#include <fstream>
#include "view_shared_image.h"

namespace Ui {
class Feed;
}

class Feed : public QDialog
{
    Q_OBJECT

public slots:
//    void reject();


public:
    explicit Feed(QWidget *parent = nullptr);
    ~Feed();

private slots:
    void on_search_btn_clicked();

    void on_profile_btn_clicked();

    void on_logout_btn_clicked();

    void on_pushButton_view_clicked();

    void on_pushButton_view_2_clicked();

    void on_notifications_bttn_clicked();

private:
    Ui::Feed *ui;
    std::vector<std::string> getNames(std::string);
    std::vector<std::string> shared_with_users;
    std::vector<std::string> images_user;
};

#endif // FEED_H
