#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <QDialog>
#include "globalvar.h"
#include "InterfaceDefintion.h"

namespace Ui {
class notifications;
}

class notifications : public QDialog
{
    Q_OBJECT

public:
    explicit notifications(QWidget *parent = nullptr);
    ~notifications();

private slots:
    void on_pushButton_handle_clicked();

    void on_profile_btn_clicked();

    void on_feed_btn_clicked();

    void on_notifications_bttn_clicked();

    void on_logout_btn_clicked();

private:
    Ui::notifications *ui;
    std::vector<RemoteImageReference> pending_requests;
    std::vector<uint32_t> no_views;
    std::vector<RemoteImageReference> accepted_requests;
    std::vector<uint32_t> agreed_no_views;
    std::string msg;
    std::vector <int> indices_more_views;
    std::vector <int> indices_agreed_views;

};

#endif // NOTIFICATIONS_H
