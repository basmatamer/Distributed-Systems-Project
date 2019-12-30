#ifndef VIEW_STATISTICS_H
#define VIEW_STATISTICS_H

#include <QDialog>
#include <map>
#include "InterfaceDefintion.h"
#include "proxyFunctions.hpp"
#include "profile.h"

namespace Ui {
class view_statistics;
}

class view_statistics : public QDialog
{
    Q_OBJECT

public:
    explicit view_statistics(QWidget *parent = nullptr);
    ~view_statistics();

private slots:
    void on_pushButton_view_clicked();

    void on_pushButton_back_clicked();

private:
    Ui::view_statistics *ui;
    std::map<std::string , std::vector<ImageUsersViews>> statisticsMap;
    std::vector<Log_entry> statisticsLog;
    std::vector<RemoteImageReference> accepted_requets;
    std::vector<uint32_t> agreed_no_views;
};

#endif // VIEW_STATISTICS_H
