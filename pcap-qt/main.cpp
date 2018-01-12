#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /* スプラッシュウィンドウの表示 */
    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/pcap-qt_Logo.png"));
    splash->show();

    /* メインウィンドウの表示 */
    MainWindow w;
    w.showVWindow();
    w.show();
    w.start();

    /* スプラッシュウィンドウの削除 */
    splash->finish(&w);
    delete splash;

    return app.exec();
}
