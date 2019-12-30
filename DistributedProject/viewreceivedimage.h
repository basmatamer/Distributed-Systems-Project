#ifndef VIEWRECEIVEDIMAGE_H
#define VIEWRECEIVEDIMAGE_H

#include "image.h"
#include <vector>
#include <string>
#include "InterfaceDefintion.h"
#include <offeredservices.h>

#include <QDialog>

namespace Ui {
class ViewReceivedImage;
}

class ViewReceivedImage : public QDialog
{
    Q_OBJECT

public slots:
//    void reject();

public:
    explicit ViewReceivedImage(std::vector<ImageReference>, string path, int remaining_views, QWidget *parent = nullptr);
    ~ViewReceivedImage();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ViewReceivedImage *ui;
    std::vector<ImageReference> r;
};

#endif // VIEWRECEIVEDIMAGE_H
