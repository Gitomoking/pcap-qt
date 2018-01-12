#include "logsavedialog.h"

LogSaveDialog::LogSaveDialog()
{
    // ウィジットの配置とコネクションの設定
    nameLabel = new QLabel(tr("ログ名を入力(25文字以内)"));
    nameEdit = new QLineEdit;
    saveButton = new QPushButton(tr("保存"));
    cancelButton = new QPushButton(tr("キャンセル"));

    saveButton->setDisabled(true);  // ログ名が入力されたときクリック可能にする

    nameEdit->setMaxLength(25); // 最大入力長25文字に設定

    connect(nameEdit, SIGNAL(textEdited(QString)),
            this, SLOT(changeButtonState()));
    connect(saveButton, SIGNAL(clicked(bool)),
            this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked(bool)),
            this, SLOT(reject()));

    // レイアウトの設定
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit);
    mainLayout->addWidget(saveButton);
    mainLayout->addWidget(cancelButton);

    setLayout(mainLayout);
    resize(500, 50);
    setWindowTitle(tr("ログの保存"));
}

QString LogSaveDialog::logName()
{
    // 入力文字列を取得して返す
    QString name = nameEdit->text();
    return name;
}

void LogSaveDialog::changeButtonState()
{
    if (nameEdit->text().count()) {
        // 入力されている場合
        if (!saveButton->isEnabled()) // 保存ボタンをクリック可能にする
            saveButton->setEnabled(true);
    } else {    // 入力されていない場合
        // 保存ボタンをクリック不可にする
        saveButton->setDisabled(true);
    }
}
