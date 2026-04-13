//
// Created by Candy on 4/13/26.
//

#ifndef SOUL_MAINWINDOW_HPP
#define SOUL_MAINWINDOW_HPP

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
};


#endif //SOUL_MAINWINDOW_HPP
