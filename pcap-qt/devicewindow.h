#ifndef DEVICEWINDOW_H
#define DEVICEWINDOW_H

#include <QtWidgets>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QPushButton>

class DeviceWindow : public QDialog
{
    Q_OBJECT

public:
    DeviceWindow();
    void setDeviceData(QList<QStringList> &data);
    int deviceIndex() { return current; }

private slots:
    void deviceClicked(QModelIndex index);
    void deviceDoubleCliecked(QModelIndex index);

private:

    QAbstractItemModel *model;
    QList<QStringList> deviceList;
    QPushButton *cancelButton;
    QPushButton *okButton;
    int current;
};

#endif // DEVICEWINDOW_H
