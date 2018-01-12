#ifndef FILTEREXPRESSIONWINDOW_H
#define FILTEREXPRESSIONWINDOW_H

#include <QWidget>

class QLabel;
class QTextEdit;
class QPushButton;

class FilterExpressionWindow : public QWidget
{
    Q_OBJECT

public:
    FilterExpressionWindow(QString &data);

signals:
    void changeFilter();

private slots:
    void emitChangingFilter();

private:
    QLabel *filterLabel;
    QTextEdit *filterText;
    QPushButton *updateButton;
    QPushButton *closeButton;
};

#endif // FILTEREXPRESSIONWINDOW_H
