#include "searchdialog.h"
#include <QHBoxLayout>

SearchDialog::SearchDialog()
{
    count = 0;
    now = 0;

    searchLabel = new QLabel(tr("検索文字列"));
    searchEdit = new QLineEdit;
    searchButton = new QPushButton(tr("検索"));
    previousButton = new QPushButton(tr("<"));
    nextButton = new QPushButton(tr(">"));
    commentLabel = new QLabel;
    closeButton = new QPushButton(tr("終了"));

    previousButton->setDisabled(true);
    nextButton->setDisabled(true);
    commentLabel->setFont(QFont("Arial", 8));   // フォント設定

    connect(searchButton, SIGNAL(clicked(bool)),
            this, SLOT(searchDone()));
    connect(previousButton, SIGNAL(clicked(bool)),
            this, SLOT(previous()));
    connect(nextButton, SIGNAL(clicked(bool)),
            this, SLOT(next()));
    connect(closeButton, SIGNAL(clicked(bool)),
            this, SLOT(emitHided()));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(searchLabel);
    mainLayout->addWidget(searchEdit);
    mainLayout->addWidget(searchButton);
    mainLayout->addWidget(previousButton);
    mainLayout->addWidget(nextButton);
    mainLayout->addWidget(commentLabel);
    mainLayout->addWidget(closeButton);
    setLayout(mainLayout);
    setWindowTitle("検索");
    resize(600, 50);
}

void SearchDialog::emitHided()
{
    // ウィンドウ初期化
    count = 0;
    now = 0;
    data.clear();
    searchEdit->clear();
    commentLabel->clear();
    previousButton->setDisabled(true);
    nextButton->setDisabled(true);
    hide();
    emit hided();   // hidedシグナル
}

QString SearchDialog::searchData()
{
    return data;
}

void SearchDialog::setSearchResult(int row)
{
    count = row;
    now = 1;
    QString result;
    result = QString::number(now) + "/";
    result += QString::number(count);
    commentLabel->setText(result);

    if (count) {    // 検索結果が1つ以上あるなら
        nextButton->setEnabled(true);   // next可能
        previousButton->setEnabled(true);   // previous可能
    }
}

void SearchDialog::setCurrentRow(int nowrow)
{
    now = nowrow + 1;
    QString result;
    result = QString::number(now) + "/";
    result += QString::number(count);
    commentLabel->setText(result);
}

void SearchDialog::searchDone()
{
    data = searchEdit->text();
    if (data.count()) {
        emit searchButtonClicked();
    } else {
        previousButton->setDisabled(true);
        nextButton->setDisabled(true);
        commentLabel->setText(tr("文字列を入力してください"));
    }
}

void SearchDialog::previous()
{
    QString result;
    if (now > 1) {
        now--;
        result = QString::number(now) + "/";
        result += QString::number(count);
        commentLabel->setText(result);
    } else {
        now = count;
        result = QString::number(now) + "/";
        result += QString::number(count);
        commentLabel->setText(result);
    }
    emit previousButtonClicked();
}

void SearchDialog::next()
{
    QString result;
    if (now < count) {
        now++;
        result = QString::number(now) + "/";
        result += QString::number(count);
        commentLabel->setText(result);
    } else {
        now = 1;
        result = QString::number(now) + "/";
        result += QString::number(count);
        commentLabel->setText(result);
    }
    emit nextButtonClicked();
}
