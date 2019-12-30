#ifndef ENTER_USERNAME_H
#define ENTER_USERNAME_H

#include <QDialog>

namespace Ui {
class enter_username;
}

class enter_username : public QDialog
{
    Q_OBJECT

public:
    explicit enter_username(QWidget *parent = nullptr);
    ~enter_username();

private:
    Ui::enter_username *ui;
};

#endif // ENTER_USERNAME_H
