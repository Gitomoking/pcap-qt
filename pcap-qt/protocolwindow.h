#ifndef PROTOCOLWINDOW_H
#define PROTOCOLWINDOW_H

#include <QWidget>
#include <QStandardItemModel>

class ProtocolWindow : public QWidget
{
    Q_OBJECT

public:
    ProtocolWindow();
    void setData(QList<QStringList> &data);
    void setTitle(QString &title);

private:
    void setupModel();
    void setupViews();

    QAbstractItemModel *traModel;
    QAbstractItemModel *appModel;
};

#endif // PROTOCOLWINDOW_H
