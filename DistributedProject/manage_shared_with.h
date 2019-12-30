#ifndef MANAGE_SHARED_WITH_H
#define MANAGE_SHARED_WITH_H
#include "InterfaceDefintion.h"
#include "proxyFunctions.hpp"
#include <QDialog>
#include <map>

namespace Ui {
class manage_shared_with;
}

class manage_shared_with : public QDialog
{
    Q_OBJECT

public:
    explicit manage_shared_with(std::string image_name, QWidget *parent = nullptr);
    ~manage_shared_with();

private slots:
    void on_pushButton_back_clicked();

    void on_pushButton_update_2_clicked();

private:
    Ui::manage_shared_with *ui;
    std::vector<RemoteImageReference> accepted_requests;
    std::vector<uint32_t> no_views;
    std::string image_name;
    std::vector<uint32_t> indices;
    std::map<std::string , std::vector<ImageUsersViews> > statisticsMap;
};

#endif // MANAGE_SHARED_WITH_H
