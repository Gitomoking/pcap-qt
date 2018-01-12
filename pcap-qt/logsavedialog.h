#ifndef LOGSAVEDIALOG_H
#define LOGSAVEDIALOG_H

#include <QDialog>
#include <QtWidgets>

class LogSaveDialog : public QDialog
{
    Q_OBJECT

public:
    LogSaveDialog();
    ~LogSaveDialog() {}
    QString logName();

private slots:
    void changeButtonState();

private:
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QPushButton *saveButton;
    QPushButton *cancelButton;
};

#endif // LOGSAVEDIALOG_H
