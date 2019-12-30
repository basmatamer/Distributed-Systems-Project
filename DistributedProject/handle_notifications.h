#ifndef HANDLE_NOTIFICATIONS_H
#define HANDLE_NOTIFICATIONS_H

#include <QDialog>
#include "proxyFunctions.hpp"
#include <iostream>
#include <vector>
#include "notifications.h"

namespace Ui {
class handle_notifications;
}

class handle_notifications : public QDialog
{
    Q_OBJECT

public:handle_notifications(std::string msg,
                            bool views_indicator,
                            std::vector<RemoteImageReference> pending_requests,
                            std::vector<uint32_t> no_views,
                            unsigned int index,
                            QWidget *parent = nullptr);handle_notifications(QWidget *parent = nullptr);
    ~handle_notifications();

private slots:
    void on_pushButton_accept_clicked();

    void on_pushButton_deny_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::handle_notifications *ui;
    std::vector<RemoteImageReference> pending_requests;
   std::vector<uint32_t> no_views;
   unsigned int position;
   bool views_indicator;
   void updatePendingRequestsLog();
};

#endif // HANDLE_NOTIFICATIONS_H
