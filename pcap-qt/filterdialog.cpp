#include "filterdialog.h"
#include <QtWidgets>

FilterDialog::FilterDialog()
{
    filExp.clear();

    tabWidget = new QTabWidget;
    hostTab = new HostTab;  // ホストタブの生成
    portTab = new PortTab;  // ポート番号タブの生成
    protoTab = new ProtocolTab; // プロトコルタブの生成

    tabWidget->addTab(hostTab, tr("ホスト"));
    tabWidget->addTab(portTab, tr("ポート番号"));
    tabWidget->addTab(protoTab, tr("プロトコル"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                     QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptFilter()));
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tabWidget);
    layout->addWidget(buttonBox);
    setLayout(layout);

    setWindowTitle(tr("フィルタ設定"));
}

void FilterDialog::acceptFilter()
{
    QString hostExp = hostTab->filterExpression(); // ホストフィルタ式
    QString portExp = portTab->filterExpression(); // ポート番号フィルタ式
    QString protoExp = protoTab->filterExpression(); // プロトコルフィルタ式

    if (hostExp.count()) { // 先頭がホストフィルタ式の場合
        filExp += hostExp;
        if (portExp.count())
            filExp += tr(" and ") + portExp;
        if (protoExp.count())
            filExp += tr(" and ") + protoExp;
    } else if (portExp.count()) { // 先頭がポート番号フィルタ式の場合
        filExp += portExp;
        if (protoExp.count())
            filExp += tr(" and ") + protoExp;
    } else // 先頭がプロトコルフィルタ式の場合
        filExp += protoExp;

    if (filExp.count()) filExp += tr(" and ");
    filExp += tr("(ip or arp)");

    accept();
}

HostTab::HostTab()
{
    hostNameLabel = new QLabel(tr("ホスト名"));
    hostEdit = new QLineEdit();
    hostNameExampleLabel = new QLabel(tr("・コンマ ',' または空白 ' ' で複数指定\n"
                                         "(例) 192.168.2.111,201.10.1.19"));
    hostNameExampleLabel->setFont(QFont("Arial", 8)); // フォントの設定

    hostOptions = new QGroupBox(tr("オプション"));
    srcdstOption = new QRadioButton(tr("送信元／宛先ホスト"));
    srcOption = new QRadioButton(tr("送信元ホスト"));
    dstOption = new QRadioButton(tr("宛先ホスト"));
    noOption = new QRadioButton(tr("ホストフィルタを設定しない"));

    srcdstOption->setChecked(true);

    QVBoxLayout *optionsLayout = new QVBoxLayout;
    optionsLayout->addWidget(srcdstOption);
    optionsLayout->addWidget(srcOption);
    optionsLayout->addWidget(dstOption);
    optionsLayout->addWidget(noOption);
    hostOptions->setLayout(optionsLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(hostNameLabel);
    mainLayout->addWidget(hostEdit);
    mainLayout->addWidget(hostNameExampleLabel);
    mainLayout->addWidget(hostOptions);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

QString HostTab::filterExpression()
{
    if (noOption->isChecked())  // フィルタ設定なしの場合
        return tr("");

    QString host = hostEdit->text(); // ホスト名
    if (!host.count()) // ホスト名が入力されてない場合
        return tr("");

    if (host.contains(tr("，")))
        host.replace(tr("，"), tr(",")); // "，"を","に変換
    if (host.contains(tr(" ")))
        host.replace(tr(" "), tr(",")); // " "を","に変換
    if (host.contains(tr("　")))
        host.replace(tr("　"), tr(",")); // "　"を","に変換

    QString exp("");
    QStringList hosts = host.split(","); // ","ごとに分割
    int size = hosts.count();
    for (int i = 0; i < size; i++) { // すべてのホスト名について
        if (!hosts[i].count()) continue;
        if (exp.count()) exp += " or ";
        else exp += "(";
        if (srcdstOption->isChecked()) // 送信元／宛先の場合
            exp += tr("host ");
        if (srcOption->isChecked()) // 送信元の場合
            exp += tr("src host ");
        if (dstOption->isChecked()) // 宛先の場合
            exp += tr("dst host ");
        exp += hosts[i]; // ホスト名の追加
    }
    if (exp.count()) exp += ")";
    return exp; // フィルタ式を返す
}

PortTab::PortTab()
{
    portNameLabel = new QLabel(tr("ポート番号"));
    portEdit = new QLineEdit();
    portNameExampleLabel = new QLabel(tr("・コンマ ',' または空白 ' ' で複数指定\n"
                                         "・ハイフン '-' でポート番号の範囲指定\n"
                                         "(例) 50,137-139 465"));
    portNameExampleLabel->setFont(QFont("Arial", 8)); // フォントの設定

    portChoices = new QGroupBox(tr("アプリケーションプロトコル"));
    dnsBox = new QCheckBox(tr("DNS"));
    dnsBox->setChecked(true);
    httpBox = new QCheckBox(tr("HTTP(S)"));
    httpBox->setChecked(true);
    ftpBox = new QCheckBox(tr("FTP(S)"));
    ftpBox->setChecked(true);
    ntpBox = new QCheckBox(tr("NTP"));
    ntpBox->setChecked(true);
    dhcpBox = new QCheckBox(tr("DHCP"));
    dhcpBox->setChecked(true);
    telnetBox = new QCheckBox(tr("Telnet"));
    telnetBox->setChecked(true);
    sshBox = new QCheckBox(tr("SSH"));
    sshBox->setChecked(true);
    portChoices->setCheckable(true);
    portChoices->setChecked(true);

    portOptions = new QGroupBox(tr("オプション"));
    srcdstOption = new QRadioButton(tr("送信元／宛先ポート番号"));
    srcOption = new QRadioButton(tr("送信元ポート番号"));
    dstOption = new QRadioButton(tr("宛先ポート番号"));
    noOption = new QRadioButton(tr("ポート番号フィルタを設定しない"));
    srcdstOption->setChecked(true);

    QVBoxLayout *optionsLayout = new QVBoxLayout;
    optionsLayout->addWidget(srcdstOption);
    optionsLayout->addWidget(srcOption);
    optionsLayout->addWidget(dstOption);
    optionsLayout->addWidget(noOption);
    portOptions->setLayout(optionsLayout);

    QGridLayout *checkBoxLayout = new QGridLayout;
    checkBoxLayout->addWidget(dnsBox, 0, 0);
    checkBoxLayout->addWidget(httpBox, 0, 1);
    checkBoxLayout->addWidget(ftpBox, 1, 0);
    checkBoxLayout->addWidget(ntpBox, 1, 1);
    checkBoxLayout->addWidget(dhcpBox, 2, 0);
    checkBoxLayout->addWidget(telnetBox, 2, 1);
    checkBoxLayout->addWidget(sshBox, 3, 0);
    portChoices->setLayout(checkBoxLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(portNameLabel);
    mainLayout->addWidget(portEdit);
    mainLayout->addWidget(portNameExampleLabel);
    mainLayout->addWidget(portChoices);
    mainLayout->addWidget(portOptions);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

QString PortTab::filterExpression()
{
    if (noOption->isChecked()) // フィルタ設定なしの場合
        return tr("");

    QString exp("");
    bool usrflag = false;

    QString port = portEdit->text();
    if (port.count()) { // ユーザ入力がある場合
        if (port.contains(tr("，")))
            port.replace(tr("，"), tr(",")); // "，"を","に変換
        if (port.contains(tr(" ")))
            port.replace(tr(" "), tr(",")); // " "を","に変換
        if (port.contains(tr("　")))
            port.replace(tr("　"), tr(",")); // "　"を","に変換

        QStringList ports = port.split(","); // ポート番号のリスト
        int size = ports.count();
        for (int i = 0; i < size; i++) { // すべてのポート番号について
            if (!ports[i].count()) continue;
            if (exp.count()) exp += tr(" or ");
            else exp += tr("(");
            if (srcdstOption->isChecked()) // 送信元／宛先の場合
                exp += tr("port");
            if (srcOption->isChecked()) // 送信元の場合
                exp += tr("src port");
            if (dstOption->isChecked()) // 宛先の場合
                exp += tr("dst port");
            if (ports[i].contains("-")) // 範囲指定の場合
                exp += tr("range ");
            else exp += tr(" ");
            exp += ports[i]; // ポート番号の追加
        }
        if (exp.count()) {
            exp += tr(")");
            usrflag = true;
        }
    }

    if (portChoices->isChecked()) { // プロトコル選択ありの場合
        if (dnsBox->isChecked()) { // DNSオン
            if (srcOption->isChecked()) { // 送信元の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("src port 53");
            } else if (dstOption->isChecked()) { // 宛先の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("dst port 53");
            } else if (usrflag) {
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("port 53");
            }
        } else { // DNSオフ
            if (exp.count()) exp +=  tr(" and ");
            exp += tr("not port 53");
        }
        if (httpBox->isChecked()) { // HTTPオン
            if (srcOption->isChecked()) { // 送信元の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("src port 80 or src port 443");
            } else if (dstOption->isChecked()) { // 宛先の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("dst port 80 or dst port 443");
            } else if (usrflag) {
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("port 80 or port 443");
            }
        } else { // HTTPオフ
            if (exp.count()) exp +=  tr(" and ");
            exp += tr("not port 80 and not port 443");
        }
        if (ftpBox->isChecked()) { // FTPオン
            if (srcOption->isChecked()) { // 送信元の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("src portrange 20-21 or "
                          "src portrange 989-990");
            } else if (dstOption->isChecked()) { // 宛先の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("dst portrange 20-21 or "
                          "dst portrange 989-990");
            } else if (usrflag) {
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("portrange 20-21 or portrange 989-990");
            }
        } else { // FTPオフ
            if (exp.count()) exp +=  tr(" and ");
            exp += tr("not portrange 20-21 and "
                      "not portrange 989-990");
        }
        if (ntpBox->isChecked()) { // NTPオン
            if (srcOption->isChecked()) { // 送信元の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("src port 123");
            } else if (dstOption->isChecked()) { // 宛先の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("dst port 123");
            } else if (usrflag) {
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("port 123");
            }
        } else { // NTPオフ
            if (exp.count()) exp +=  tr(" and ");
            exp += tr("not port 123");
        }
        if (dhcpBox->isChecked()) { // DHCPオン
            if (srcOption->isChecked()) { // 送信元の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("src portrange 67-68");
            } else if (dstOption->isChecked()) { // 宛先の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("dst portrange 67-68");
            } else if (usrflag) {
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("portrange 67-68");
            }
        } else { // DHCPオフ
            if (exp.count()) exp +=  tr(" and ");
            exp += tr("not portrange 67-68");
        }
        if (telnetBox->isChecked()) { // Telnetオン
            if (srcOption->isChecked()) { // 送信元の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("src port 23");
            } else if (dstOption->isChecked()) { // 宛先の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("dst port 23");
            } else if (usrflag) {
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("port 23");
            }
        } else { // Telnetオフ
            if (exp.count()) exp +=  tr(" and ");
            exp += tr("not port 23");
        }
        if (sshBox->isChecked()) { // SSHオン
            if (srcOption->isChecked()) { // 送信元の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("src port 22");
            } else if (dstOption->isChecked()) { // 宛先の場合
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("dst port 22");
            } else if (usrflag) {
                if (exp.count()) exp +=  tr(" or ");
                exp += tr("port 22");
            }
        } else { // SSHオフ
            if (exp.count()) exp +=  tr(" and ");
            exp += tr("not port 22");
        }
    } else { // アプリケーションプロトコルオフ
        if (exp.count()) exp +=  tr(" and ");
        exp += tr("not portrange 20-23 and not port 53 and "
                  "not portrange 67-68 and not port 80 and "
                  "not port 123 and not port 443 and "
                  "not portrange 989-990");
    }

    return exp; // フィルタ式を返す
}

ProtocolTab::ProtocolTab()
{
    protoLabel = new QLabel(tr("プロトコル"));
    arpBox = new QCheckBox(tr("ARP"));
    arpBox->setChecked(true);
    tcpBox = new QCheckBox(tr("TCP"));
    tcpBox->setChecked(true);
    udpBox = new QCheckBox(tr("UDP"));
    udpBox->setChecked(true);
    icmpBox = new QCheckBox(tr("ICMP"));
    icmpBox->setChecked(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(protoLabel);
    mainLayout->addWidget(arpBox);
    mainLayout->addWidget(tcpBox);
    mainLayout->addWidget(udpBox);
    mainLayout->addWidget(icmpBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

QString ProtocolTab::filterExpression()
{
    QString exp("");
    if (!arpBox->isChecked()) { // ARPオン
        if (exp.count()) exp +=  tr(" and ");
        exp += tr("not arp");
    }
    if (!tcpBox->isChecked()) { // TCPオン
        if (exp.count()) exp +=  tr(" and ");
        exp += tr("not tcp");
    }
    if (!udpBox->isChecked()) { // UDPオン
        if (exp.count()) exp +=  tr(" and ");
        exp += tr("not udp");
    }
    if (!icmpBox->isChecked()) { // ICMPオン
        if (exp.count()) exp +=  tr(" and ");
        exp += tr("not icmp");
    }
    return exp;
}
