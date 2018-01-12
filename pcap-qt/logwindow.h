#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include "dbaccess.h"
#include <QWidget>
#include <QtWidgets>
#include <QStandardItemModel>
#include <QTableView>

const QString flagColor("#9ca7e2"); // 背景色

class LogWindow : public QDialog
{
    Q_OBJECT

public:
    LogWindow();
    void setDBaccess(DBAccess *dbaccess);
    void initialize();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif

private slots:
    void dataUpdated();
    void changeFlag();
    void updateDatabase();
    void dataClicked(QModelIndex index);
    void deleteLog();
    void showProtocolWindow();
    void showTrafficWindow();
    void showNodeSetWindow();
    void hideEvent();

private:
    void loadData();
    void createActions();

    bool connection;
    DBAccess *DB;
    int currentRow;
    QList<QStringList> logList;
    QMap<int, QStringList> updateMap;
    QMap<int, QList<QStringList> > protoMap;
    QMap<int, QMap<int, QList<double_t> > > trafficMap;
    QMap<int, QList<QStringList> > nodesMap;

    QAbstractItemModel *model;
    QPushButton *updateButton;
    QPushButton *closeButton;

    QAction *protoAction;
    QAction *trafficAction;
    QAction *nodesAction;
    QAction *flagAction;
    QAction *deleteAction;
};

#endif // LOGWINDOW_H
