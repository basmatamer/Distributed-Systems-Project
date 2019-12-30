#ifndef OFFEREDSERVICES_H
#define OFFEREDSERVICES_H
#include <vector>
#include "InterfaceDefintion.h"
#include <QDialog>
#include <string>
#include "image.h"
#include "search.h"

namespace Ui {
class OfferedServices;
}

class OfferedServices : public QDialog
{
    Q_OBJECT

public slots:
//    void reject();

public:
    explicit OfferedServices(std::vector<ImageReference>, QWidget *parent = nullptr);
    ~OfferedServices();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::OfferedServices *ui;
    std::vector<ImageReference> r;
};

#endif // OFFEREDSERVICES_H
