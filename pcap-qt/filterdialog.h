#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>

class QLabel;
class QRadioButton;
class QLineEdit;
class QListView;
class QStringListModel;
class QTabWidget;
class QDialogButtonBox;
class QGroupBox;
class QCheckBox;

class HostTab : public QWidget
{
    Q_OBJECT
public:
    explicit HostTab();
    QString filterExpression();

private:
    QLabel *hostNameLabel;
    QLineEdit *hostEdit;
    QLabel *hostNameExampleLabel;
    QGroupBox *hostOptions;
    QRadioButton *srcdstOption;
    QRadioButton *srcOption;
    QRadioButton *dstOption;
    QRadioButton *noOption;
};

class PortTab : public QWidget
{
    Q_OBJECT
public:
    explicit PortTab();
    QString filterExpression();

private:
    QLabel *portNameLabel;
    QLineEdit *portEdit;
    QLabel *portNameExampleLabel;
    QGroupBox *portChoices;
    QGroupBox *portOptions;
    QCheckBox *dnsBox;
    QCheckBox *httpBox;
    QCheckBox *ftpBox;
    QCheckBox *ntpBox;
    QCheckBox *dhcpBox;
    QCheckBox *telnetBox;
    QCheckBox *sshBox;
    QRadioButton *srcdstOption;
    QRadioButton *srcOption;
    QRadioButton *dstOption;
    QRadioButton *noOption;
};

class ProtocolTab : public QWidget
{
    Q_OBJECT
public:
    explicit ProtocolTab();
    QString filterExpression();
private:
    QLabel *protoLabel;
    QCheckBox *arpBox;
    QCheckBox *tcpBox;
    QCheckBox *udpBox;
    QCheckBox *icmpBox;
};

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog();
    QString data() const { return filExp; }

private slots:
    void acceptFilter();

private:
    QTabWidget *tabWidget;  // タブウィジット
    HostTab *hostTab;  // ホストタブ
    PortTab *portTab;  // ポート番号タブ
    ProtocolTab *protoTab; // プロトコルタブ
    QDialogButtonBox *buttonBox;    // 確定ボタン

    QString filExp;   // フィルタ式
};

#endif // FILTERDIALOG_H
