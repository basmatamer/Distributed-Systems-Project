#ifndef REQUEST_MORE_VIEWS_H
#define REQUEST_MORE_VIEWS_H

#include <QDialog>
#include "InterfaceDefintion.h"
#include <string>
#include <vector>
#include <QMessageBox>
#include "proxyFunctions.hpp"

namespace Ui {
class request_more_views;
}

class request_more_views : public QDialog
{
    Q_OBJECT

public:
    explicit request_more_views(std::vector<ImageReference> ret,int i,QWidget *parent = nullptr);
    ~request_more_views();

private slots:
    void on_pushButton_clicked();

    void on_request_button_clicked();

private:
    Ui::request_more_views *ui;
    std::vector<ImageReference> ret;
    int position;
};

#endif // REQUEST_MORE_VIEWS_H
