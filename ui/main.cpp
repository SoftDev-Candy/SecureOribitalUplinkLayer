//
// Created by Candy on 4/13/26.
//

#include <QApplication>

#include "mainwindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
