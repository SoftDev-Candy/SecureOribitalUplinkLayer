#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class mainwindow;
}

class mainwindow : public QWidget
{
    Q_OBJECT

public:
    explicit mainwindow(QWidget *parent = nullptr );
    ~mainwindow();

    //Look up data in DB and read it and then set values in the respective columns...
    void RefreshTelemetryView();
    const char* LoadDatabase();

private:
    Ui::mainwindow *ui;
};

#endif // MAINWINDOW_H
