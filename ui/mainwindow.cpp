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
    //Find size of Telemetry for rows
    int rows = 0;


    //find data in table
    const char* sql  = "SELECT Satellite_name, sequence , battery, temperature, received_ms, timestamp_ms "
                       "FROM Telemetry "
                       "ORDER BY id DESC";

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        //we are using while here as sqlite knows how man rows it has we just need to read all of them
            while (sqlite3_step(stmt) == SQLITE_ROW )
            {

                //Read the data stored in database col wise
                const unsigned char* satName = sqlite3_column_text(stmt, 0);
                sqlite3_int64 sequence = sqlite3_column_int64(stmt, 1);
                double battery = sqlite3_column_double(stmt, 2);
                double temperature = sqlite3_column_double(stmt, 3);
                sqlite3_int64 receivedMs = sqlite3_column_int64(stmt, 4);
                sqlite3_int64 timestampMs = sqlite3_column_int64(stmt, 5);
                int64_t latency = receivedMs - timestampMs;

                if(satName != nullptr)
                {
                    ui->SatelliteName_Label->setText(reinterpret_cast<const char*>(satName));
                }

                //TODO -- Add a condition in Sender for sat_battery so we can deal with underflow condition//
                ui->Battery_Data->setText(QString::number(battery));
                ui->Temprature_Data->setText(QString::number(temperature));
                ui->Latency_Data->setText(QString::number(latency));
                ui->DatabaseTable->setItem(rows,0,new QTableWidgetItem(QString::number(sequence)));
                ui->DatabaseTable->setItem(rows,1,new QTableWidgetItem(QString::number(timestampMs)));
                ui->DatabaseTable->setItem(rows,2,new QTableWidgetItem(QString::number(receivedMs)));

                //TODO --- Calculate packet size in the backend and send it here//


rows++;

            if (sqlite3_step(stmt) == SQLITE_DONE)
            {
                std::cerr<<"No data in the table"<<"\n";
                break;
            }
        }

    }

    //Finalise the statement and then close the database when work is done//
    sqlite3_finalize(stmt);
    sqlite3_close(DB);

}

const char * mainwindow::LoadDatabase()
{
    //FIXME  - Separate database logic later here //
}
