#include "mainwindow.h"
#include <QApplication>
#include <QWidget>
#include <QObject>
#include <QDesktopWidget>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    MainWindow w;

    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(desktop.primaryScreen());
    QPoint center = rect.center();
    int x = center.x() - (w.width()/2);
    int y = center.y() - (w.height()/2);
    center.setX(x);
    center.setY(y);
    w.move(center);
    w.setFixedSize(w.width(), w.height());
    w.show();

    return a.exec();
}
