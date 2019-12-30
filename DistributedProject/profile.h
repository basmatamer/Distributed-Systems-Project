#ifndef PROFILE_H
#define PROFILE_H

#include <QDialog>

namespace Ui {
class Profile;
}

class Profile : public QDialog
{
    Q_OBJECT

public slots:
//    void reject();

public:
    explicit Profile(QWidget *parent = nullptr);
    ~Profile();

private slots:
    void on_feed_btn_clicked();

    void on_search_btn_clicked();

    void on_pushButton_import_clicked();

    void on_pushButton_view_image_clicked();

    void on_logout_btn_clicked();

    void on_notifications_bttn_clicked();

    void on_pushButton_statistics_clicked();

private:
    Ui::Profile *ui;
    std::vector<std::string> imported_images;

    std::vector<std::string> getImageNames();
};

#endif // PROFILE_H
