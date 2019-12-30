#ifndef VIEW_SHARED_IMAGE_H
#define VIEW_SHARED_IMAGE_H

#include <QDialog>
#include "request_more_views.h"
#include "feed.h"
#include "InterfaceDefintion.h"
#include "globalvar.h"

namespace Ui {
class view_shared_image;
}

class view_shared_image : public QDialog
{
    Q_OBJECT

public:
    explicit view_shared_image(std::string path, int remaining_views, std::string shared_username, std::string image_name, bool locked,
                               QWidget *parent = nullptr);
    ~view_shared_image();

private slots:
    void on_pushButton__clicked();

    void on_pushButton_back_clicked();

private:
    Ui::view_shared_image *ui;
    std::string shared_username;
    std::string image_name;
};

#endif // VIEW_SHARED_IMAGE_H
