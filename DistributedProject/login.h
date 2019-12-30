#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

private slots:
    void on_pushButton_login_clicked();
    void on_pushButton_signup_clicked();

private:
    Ui::Login *ui;
};















































































#endif // MAINWINDOW_H
