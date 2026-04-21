//
// Created by Candy on 4/14/2026.
//

#include "mainwindow.h"
#include<QApplication>

int main(int argc , char *argv[])
{
    QApplication a(argc , argv);
    mainwindow window;

    window.show();
    return QCoreApplication::exec();
}