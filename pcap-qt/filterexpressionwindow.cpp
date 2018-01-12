#include "filterexpressionwindow.h"
#include <QtWidgets>

FilterExpressionWindow::FilterExpressionWindow(QString &data)
{
    filterLabel = new QLabel(tr("現在のフィルタ式"));
    filterText = new QTextEdit(data);
    updateButton = new QPushButton(tr("フィルタを変更する"));
    closeButton = new QPushButton(tr("閉じる"));

    filterText->setReadOnly(true);  // 読み取りのみ

    connect(updateButton, SIGNAL(clicked(bool)), this, SLOT(emitChangingFilter()));
    connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(filterLabel);
    mainLayout->addWidget(filterText);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("フィルタ式設定"));
}

void FilterExpressionWindow::emitChangingFilter()
{
    emit changeFilter(); // フィルタ更新シグナル
    close();
}
