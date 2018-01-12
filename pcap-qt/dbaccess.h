#ifndef DBACCESS_H
#define DBACCESS_H

#include <QtWidgets>
#include <QtSql/QSqlDatabase>

class DBAccess
{

public:
        DBAccess();

        bool createConnection();
        void closeConnection();

        bool insertLogData(QString &memo);   // ログデータの保存
        bool insertNodeSet(QList<QStringList> &nodeSetList); // ノードの組み保存
        bool insertTraffic(QMap<int, QList<double_t> > &traffficMap); // トラフィック量保存
        bool insertProtocol(QList<QStringList> &protocolList);   // プロトコル保存

        bool deleteLogData(int id); // ログデータの削除

        QList<QStringList> selectLogData(void); // ログ一覧を取得
        QList<QStringList> selectNodeSet(int id);   // ノードの組み取得
        QMap<int, QList<double_t> > selectTraffic(int id, QList<int> &nodeIDList); // トラフィック量取得
        QList<QStringList> selectProtocol(int id);  // プロトコル取得

        bool updateLogData(int id, int flag,QString memo); // ログデータの更新
private:
        int myID;
        QSqlDatabase myDB;
        QStringList protocols;
};

#endif // DBACCESS_H
