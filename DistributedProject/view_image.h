#ifndef VIEW_IMAGE_H
#define VIEW_IMAGE_H

#include <QDialog>
#include <string>

namespace Ui {
class view_image;
}

class view_image : public QDialog
{
    Q_OBJECT

public slots:
//    void reject();

public:
    explicit view_image(const std::string& image_name, QWidget *parent = nullptr);
    ~view_image();

private slots:
    void on_pushButton_back_clicked();

    void on_pushButton_offer_to_2_clicked();

private:
    Ui::view_image *ui;
    std::string this_image_name;
};

#endif // VIEW_IMAGE_H
