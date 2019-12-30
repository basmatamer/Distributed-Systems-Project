#include "enter_username.h"
#include "ui_enter_username.h"

enter_username::enter_username(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::enter_username) {
    ui->setupUi(this);
}

enter_username::~enter_username() {
    delete ui;
}
