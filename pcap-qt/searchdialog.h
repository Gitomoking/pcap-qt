#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QtWidgets>

class QLabel;
class QLineEdit;
class QPushButton;

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    SearchDialog();
    QString searchData();
    void setSearchResult(int row);
    void setCurrentRow(int nowrow);

signals:
    void hided();
    void searchButtonClicked();
    void previousButtonClicked();
    void nextButtonClicked();

private slots:
    void searchDone();
    void next();
    void previous();
    void emitHided();

private:
    QLabel *searchLabel;
    QLineEdit *searchEdit;
    QPushButton *searchButton;
    QPushButton *previousButton;
    QPushButton *nextButton;
    QLabel *commentLabel;
    QPushButton *closeButton;
    QString data;
    int count;
    int now;
};

#endif // SearchDialog_H
