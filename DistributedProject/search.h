#ifndef SEARCH_H
#define SEARCH_H

#include <QDialog>

namespace Ui {
class Search;
}

class Search : public QDialog
{
    Q_OBJECT

public slots:
//    void reject();


public:
    explicit Search(QWidget *parent = nullptr);
    ~Search();

private slots:
    void on_profile_btn_clicked();

    void on_feed_btn_clicked();

    void on_search_btn_2_clicked();

    void on_view_all_clicked();

    void on_logout_btn_clicked();

    void on_notifications_bttn_clicked();

private:
    Ui::Search *ui;
};

#endif // SEARCH_H
