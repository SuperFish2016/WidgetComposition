#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QFile style_file(":/stylesheet.qss");
    bool success = style_file.open(QFile::ReadOnly | QIODevice::Text);
    if(success) {
        a.setStyleSheet(QString::fromUtf8(style_file.readAll()));
        style_file.close();
    }
    else {
        qWarning() << "Couldn't load stylesheet: " << style_file.fileName();
    }
    return a.exec();
}
