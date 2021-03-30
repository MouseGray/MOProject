#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("System"));
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("MOProject");
    w.show();
    return a.exec();
}
