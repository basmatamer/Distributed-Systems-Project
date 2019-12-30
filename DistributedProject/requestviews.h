#ifndef REQUESTVIEWS_H
#define REQUESTVIEWS_H

#include <QDialog>
#include "InterfaceDefintion.h"

namespace Ui {
class RequestViews;
}

class RequestViews : public QDialog
{
    Q_OBJECT

public slots:
//    void reject();

public:
    explicit RequestViews(std::vector<ImageReference> ret,int i,QWidget *parent = nullptr);
    ~RequestViews();

private slots:
    void on_pushButton_clicked();

    void on_request_button_clicked();

private:
    Ui::RequestViews *ui;
    std::vector<ImageReference> r;
    unsigned int index;
};

#endif // REQUESTVIEWS_H
