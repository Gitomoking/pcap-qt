#include "dbaccess.h"
#include <QtSql/QSqlQuery>
#include <iostream>
#include <QtWidgets>

const QString arpColor("#ff7f7f");
const QString tcpColor("#00ff7f");
const QString udpColor("#00bfff");
const QString icmpColor("#b114ff");
const QString dnsColor("#adff2f");
const QString httpColor("#ffff00");
const QString httpsColor("#ffff00");
const QString ftpColor("#006400");
const QString ntpColor("#0000ff");
const QString dhcpColor("#ffc0cb");
const QString telnetColor("#ffffe0");
const QString sshColor("#ff1493");
const QString otherColor("#fafafa");


DBAccess::DBAccess()
{
    myDB = QSqlDatabase::addDatabase("QMYSQL"); // MySQLに接続
    myDB.setHostName("127.0.0.1");  // ホスト名（ローカルホスト）
    myDB.setDatabaseName("pcapdb"); // データベース名
    myDB.setUserName("root");   // ユーザ名
    myDB.setPassword("");   // ユーザのパスワード

    protocols = QStringList() << "ARP" << "TCP" << "UDP" << "ICMP"
                              << "DNS" << "HTTP" << "HTTPS" << "FTP"
                              << "NTP" << "DHCP" << "Telnet"
                              << "SSH" << "その他";
}

bool DBAccess::createConnection()
{
    return myDB.open(); // データベースとのコネクション確立
}

void DBAccess::closeConnection()
{
    myDB.close();
}

bool DBAccess::insertLogData(QString &memo)
{
    QSqlQuery countQuery;
    countQuery.exec("SELECT COUNT(*) FROM LogData");
    if (countQuery.next()) {
        int count = countQuery.value(0).toInt();
        if (count >= 30) {
            // 最も古く，フラグが立っていないデータを削除
            // すべてに立っている場合は，挿入エラーを出力
            QSqlQuery deleteQuery;
            deleteQuery.exec("DELETE FROM LogData WHERE LogID=(SELCT MAX(LogID) FROM LogData WHERE FavFlag=0)");
        }
    }

    QSqlQuery query;  // 実行するクエリ
    QString sql("INSERT INTO LogData (DateTime,Memo)"
                "VALUES (NOW(),\"");
    sql += memo + "\")";
    query.exec(sql);    // INSERT文実行

    int rows = query.numRowsAffected(); // 影響を受けた行数
    if (rows == -1) {   // エラーの場合
        std::cout<<"insertに失敗"<<std::endl;
        myID = -1;
        return false;
    } else {    // 成功の場合
        // LogIDが最も大きい（自分のログ）のログIDをSELECT
        query.exec("SELECT MAX(LogID) FROM LogData");
        if (query.next()) {
            myID = query.value(0).toInt();
            std::cout<<rows<<" 行挿入しました"<<std::endl;
            return true;
        } else {
            std::cout<<"ログIDの取得に失敗"<<std::endl;
            myID = -1;
            return false;
        }
    }
}

bool DBAccess::insertNodeSet(QList<QStringList> &nodeSetList)
{
    int rows = 0;
    int size = nodeSetList.count();
    // それぞれのノードの組みに対してINSERT文を実行する
    for (int i = 0; i < size; i++) {
        QSqlQuery query;
        QString sql("INSERT INTO NodeSet(LogID,SrcNode,DstNode,TrafficVolStat) VALUES (");
        sql += QString::number(myID) + ",\"";
        sql += nodeSetList[i][0] + "\",\"";
        sql += nodeSetList[i][1] + "\",";
        sql += nodeSetList[i][2] + ")";

        query.exec(sql);    // SQL文実行

        if (query.numRowsAffected() == -1) {    // エラーの場合
            std::cout<<"insertに失敗"<<std::endl;
            return false;
        } else rows++;  // 行数 + 1
    }

    std::cout<<rows<<"行挿入しました"<<std::endl;
    return true;
}

bool DBAccess::insertTraffic(QMap<int, QList<double_t> > &trafficMap)
{
    // ノードの組みIDリストを取得
    QStringList nodeIDList;
    QSqlQuery nodeQuery;
    QString nodeSQL("SELECT NodeSetID FROM NodeSet WHERE LogID=");
    nodeSQL += QString::number(myID);

    nodeQuery.exec(nodeSQL);
    while (nodeQuery.next()) {
        nodeIDList.append(nodeQuery.value(0).toString());  // ノードID追加
    }

    // ノードの組みIDごとにトラフィック量を追加
    int rows = 0;
    int size = nodeIDList.count();
    for (int i = 0; i < size; i++) {
        QList<double_t> traffic = trafficMap[i];
        int trasize = traffic.size();
        // トラフィック量ごとにINSERT文を実行
        for (int j = 0; j < trasize; j++) {
            QSqlQuery query;
            QString sql("INSERT INTO Traffic(LogID,NodeSetID,TrafficVol) VALUES(");
            sql += QString::number(myID) + ",";
            sql += nodeIDList[i] + ",";
            sql += QString::number(traffic[j]) + ")";

            query.exec(sql);    // SQL文実行
            if (query.numRowsAffected() == -1) {    // エラーの場合
                std::cout<<"insertに失敗"<<std::endl;
                return false;
            } else rows++;
        }
    }

    std::cout<<rows<<"行挿入しました"<<std::endl;
    return true;
}

bool DBAccess::insertProtocol(QList<QStringList> &protocolList)
{
    QSqlQuery query;
    QString sql("INSERT INTO Protocol VALUES(");
    sql += QString::number(myID);   // LogID
    int size = protocolList.count();
    for (int i = 0; i < size; i++) {
        sql += "," + protocolList[i][1];
    }
    sql += ")";
    query.exec(sql);    // SQL文実行

    int rows = query.numRowsAffected(); // 影響を受けた行数
    if (rows == -1) {   // エラーの場合
        std::cout<<"insertに失敗"<<std::endl;
        return false;
    } else    // 成功の場合
        return true;
}

bool DBAccess::deleteLogData(int id)
{
    QSqlQuery query;  // 実行するクエリ
    QString sql("delete from LogData where LogID=");    // delete文
    sql += QString::number(id);
    query.exec(sql);    // delete文実行

    int rows = query.numRowsAffected(); // 影響を受けた行数
    if (rows == -1) {   // エラーの場合
        std::cout<<"insertに失敗"<<std::endl;
        return false;
    } else {    // 成功の場合
        std::cout<<rows<<" 行変更しました"<<std::endl;
        return true;
    }
}

QList<QStringList> DBAccess::selectLogData()
{
    QSqlQuery query;  // 実行するクエリ
    query.exec("SELECT * FROM LogData");    // SELECT文実行

    QList<QStringList> result;  // 結果格納
    while (query.next()) {  // データが存在する場合
        QStringList data;   // ログデータ
        for (int i = 0; i < 4; i++)
            data.append(query.value(i).toString());
        result.append(data);    // 追加
    }

    return result;
}

QList<QStringList> DBAccess::selectNodeSet(int id)
{
    QSqlQuery query;  // 実行するクエリ
    QString sql("SELECT NodeSetID,SrcNode,DstNode,TrafficVolStat FROM NodeSet WHERE LogID=");
    sql += QString::number(id); // SELECT文

    query.exec(sql); // SQL文実行

    QList<QStringList> result;  // 結果格納
    while (query.next()) {  // データが存在する場合
        QStringList data;
        for (int i = 0; i < 4; i++)
            data.append(query.value(i).toString());
        result.append(data);
    }

    return result;
}

QMap<int, QList<double_t> > DBAccess::selectTraffic(int id, QList<int> &nodeIDList)
{
    QMap<int, QList<double_t> > result;
    int row = 0;    // QMapのキー
    foreach (const int nodesID, nodeIDList) {
        QSqlQuery query;  // 実行するクエリ
        QString sql("SELECT TrafficVol FROM Traffic WHERE LogID=");  // SELECT文
        sql += QString::number(id) + " AND NodeSetID=" + QString::number(nodesID);
        sql += " ORDER BY DataID ASC";  // データの昇順で

        query.exec(sql);    // SQL文実行

        QList<double_t> trafficList;
        while (query.next()) {
            // トラフィック量追加
            trafficList.append(query.value(0).toDouble());
        }
        result[row] = trafficList;  // 結果に追加
        row++;  // 次ノードID
    }

    return result;
}

QList<QStringList> DBAccess::selectProtocol(int id)
{
    QSqlQuery query;  // 実行するクエリ
    QString sql("SELECT * FROM Protocol WHERE LogID=");
    sql += QString::number(id);

    query.exec(sql);

    qlonglong netAll = 0;
    qlonglong appAll = 0;

    QList<QStringList> result;
    while (query.next()) {
        int size = protocols.count();
        for (int i = 0; i < size; i++) {
            QStringList data;
            data.append(protocols[i]);  // プロトコル名
            data.append(query.value(i + 1).toString()); // 数
            result.append(data);    // データ追加
            // ネットワーク合計とアプリケーション合計を計算
            if (i < 4) netAll += data[1].toInt();
            else appAll += data[1].toInt();
        }
    }
    int size = protocols.count();
    for (int i = 0; i < size; i++) {
        double rate;    // 存在比
        if (i < 4)  // ネットワークプロトコルの存在比計算
            rate = 100 * (result[i][1].toDouble()/netAll);
        else    // アプリケーションプロトコルの存在比計算
            rate = 100 * (result[i][1].toDouble()/appAll);
        result[i].append(QString::number(rate));    // 存在比追加
        if (!QString::compare(result[i][0], QString("ARP")))
            result[i].append(arpColor);
        if (!QString::compare(result[i][0], QString("TCP")))
            result[i].append(tcpColor);
        if (!QString::compare(result[i][0], QString("UDP")))
            result[i].append(udpColor);
        if (!QString::compare(result[i][0], QString("ICMP")))
            result[i].append(icmpColor);
        if (!QString::compare(result[i][0], QString("DNS")))
            result[i].append(dnsColor);
        if (!QString::compare(result[i][0], QString("HTTP")))
            result[i].append(httpColor);
        if (!QString::compare(result[i][0], QString("HTTPS")))
            result[i].append(httpsColor);
        if (!QString::compare(result[i][0], QString("FTP")))
            result[i].append(ftpColor);
        if (!QString::compare(result[i][0], QString("NTP")))
            result[i].append(ntpColor);
        if (!QString::compare(result[i][0], QString("DHCP")))
            result[i].append(dhcpColor);
        if (!QString::compare(result[i][0], QString("Telnet")))
            result[i].append(telnetColor);
        if (!QString::compare(result[i][0], QString("SSH")))
            result[i].append(sshColor);
        if (!QString::compare(result[i][0], QString("その他")))
            result[i].append(otherColor);
    }
    return result;
}

bool DBAccess::updateLogData(int id, int flag, QString memo)
{
    QSqlQuery query;
    QString sql("UPDATE LogData SET ");
    sql += "FavFlag=" + QString::number(flag);
    sql += ",Memo=\"" + memo + "\"";
    sql += " WHERE LogID=" + QString::number(id);

    query.exec(sql);
    int rows = query.numRowsAffected();
    if (rows == -1) {   // エラーの場合
        std::cout<<"updateが失敗しました"<<std::endl;
        return false;
    } else {
        std::cout<<rows<<"行更新しました"<<std::endl;
        return true;
    }
}
