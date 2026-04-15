#include "mainwindow.h"

#include <iostream>
#include <filesystem>
#include "ui_mainwindow.h"
#include "../external/sqlite/sqlite3.h"

mainwindow::mainwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainwindow)
{
    ui->setupUi(this);
}

mainwindow::~mainwindow()
{
    delete ui;
}

void mainwindow::LoadLatestTelemetry()
{
    sqlite3* DB;
    sqlite3_stmt* stmt;
    std::cout << "Current working directory: "
              << std::filesystem::current_path() << "\n";

    const char* dbpath = "C:/SOUL/cmake-build-debug/Soul.db";
    int rc = sqlite3_open(dbpath,&DB);//Call the sqlite constructor
    if (rc != SQLITE_OK)
    {
        std::cerr<<"Failed to open database ERROR  : "<<sqlite3_errmsg(DB)<<"\n";
        sqlite3_close(DB);
    }

    const char* sql  = "SELECT Satellite_name, battery, temperature, received_ms, timestamp_ms "
                       "FROM Telemetry "
                       "ORDER BY id DESC "
                       "LIMIT 1";

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {

    int stepResult = sqlite3_step(stmt);

        if (stepResult == SQLITE_ROW )
        {
            //Read the data stored in database col wise
            const unsigned char* satName = sqlite3_column_text(stmt, 0);
            double battery = sqlite3_column_double(stmt, 1);
            double temperature = sqlite3_column_double(stmt, 2);
            sqlite3_int64 receivedMs = sqlite3_column_int64(stmt, 3);
            sqlite3_int64 timestampMs = sqlite3_column_int64(stmt, 4);

            if(satName != nullptr)
            {
                ui->SatelliteName_Label->setText(reinterpret_cast<const char*>(satName));
            }
            if(battery != -1)
            {
                ui->Battery_Data->setText(QString::number(battery));
            }
            if(temperature != -1)
            {
                ui->Temprature_Data->setText(QString::number(temperature));
            }

        }
        else if (stepResult == SQLITE_DONE)
        {
            std::cerr<<"No data in the table"<<"\n";
        }


    }

    //Finalise the statement and then close the database when work is done//
    sqlite3_finalize(stmt);
    sqlite3_close(DB);

}
