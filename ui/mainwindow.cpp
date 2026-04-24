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

    //Creating a timer to run the RefreshTelemetryView function every 1000ms
    //In constructor because we want qt to handle its creation and destruction so
    //When the window is destroyed, the timer is destroyed too.
    refreshTimer = new QTimer(this);
    connect(refreshTimer , &QTimer::timeout ,this , &mainwindow::RefreshTelemetryView);
    RefreshTelemetryView();

    refreshTimer->start(1000);

}

mainwindow::~mainwindow()
{
    delete ui;
}

void mainwindow::RefreshTelemetryView()
{
    //Clear contents in the table and reset the row count back to 0//
    ui->DatabaseTable->clearContents();
    ui->DatabaseTable->setRowCount(0);

    sqlite3* DB;
    sqlite3_stmt* stmt = nullptr;

    std::cout << "Current working directory: "
                << std::filesystem::current_path() << "\n";

    const char* dbpath = "C:/SOUL/cmake-build-debug/Soul.db";
    int rc = sqlite3_open(dbpath,&DB);//Call the sqlite constructor
    if (rc != SQLITE_OK)
    {
        std::cerr<<"Failed to open database ERROR  : "<<sqlite3_errmsg(DB)<<"\n";
        sqlite3_close(DB);
        return;
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

                //Set Database row size here so we are able to store the data properly with accurate size//
                ui->DatabaseTable->insertRow(rows);

                if(rows == 0 && satName != nullptr)
                {

                    ui->SatelliteName_Label->setText(QString::fromUtf8((reinterpret_cast<const char*>(satName))));
                    //TODO -- Add a condition in Sender for sat_battery so we can deal with underflow condition//
                    ui->Battery_Data->setText(QString::number(battery ,'f',2));
                    ui->Temprature_Data->setText(QString::number(temperature ,'f' ,2 ));
                    ui->Latency_Data->setText(QString::number(latency));

                    //Calculate current time here //
                    auto duration  = std::chrono::system_clock::now().time_since_epoch();
                    //Conversion to store it in time stamp
                    sqlite3_uint64 current_time = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

                    sqlite3_uint64 age_ms = current_time - receivedMs; // Age_ms is the time to check the status of the connection

                    std::cout<<"Age ms value is : "<<age_ms<<"\n";

                    //Check if connection is stable or not and send it to the UI based on packet age//
                    if(age_ms < 3000)
                    {
                        ui->LinkStatus_Connection->setText("Connected");
                    }

                    else if(age_ms >= 3000 && age_ms <= 7000)
                    {
                        ui->LinkStatus_Connection->setText("Degraded");
                    }

                    else if (age_ms > 7000)
                    {
                        ui->LinkStatus_Connection->setText("Disconnected");
                    }

                    //Checks for Battery Health and shows it in the UI
                    if (battery < 30)
                    {
                        ui->Battery_StatusData->setText("Critical");
                    }
                    else if(battery >= 30 && battery <= 70)
                    {
                        ui->Battery_StatusData->setText("Normal");
                    }

                    else if (battery > 70)
                    {
                        ui->Battery_StatusData->setText("Good");
                    }
                    //TODO -- Add a battery temperature to logic to control battery temp if gets too hot//


                    //TODO-- Add a temperature Range check here to make sure a satellite doesnt get to hot or cold..//

                    if (temperature < 0)
                    {
                        ui->Temp_StatusData->setText("Critical Low");
                    }
                    else if (temperature < 15)
                    {
                        ui->Temp_StatusData->setText("Low");
                    }
                    else if (temperature <= 60)
                    {
                        ui->Temp_StatusData->setText("Normal");
                    }
                    else if (temperature <= 75)
                    {
                        ui->Temp_StatusData->setText("High");
                    }
                    else
                    {
                        ui->Temp_StatusData->setText("Critical High");
                    }

                }

                ui->DatabaseTable->setItem(rows,0,new QTableWidgetItem(QString::number(sequence)));
                ui->DatabaseTable->setItem(rows,1,new QTableWidgetItem(QString::number(timestampMs)));
                ui->DatabaseTable->setItem(rows,2,new QTableWidgetItem(QString::number(receivedMs)));
                //TODO --- Calculate packet size in the backend and send it here and calculate date&time to add to database as well//

                rows++;

        }
        if (rows == 0)
        {
            std::cerr<<"No data in the table"<<"\n";
            ui->SatelliteName_Label->setText("N/A");
            ui->Battery_Data->setText("N/A");
            ui->Temprature_Data->setText("N/A");
            ui->Latency_Data->setText("N/A");
        }
    }
    else
    {
        std::cerr<<"Prepare has failed , Error in file mainwindow.cpp \n";
    }

    //Finalise the statement and then close the database when work is done//
    sqlite3_finalize(stmt);
    sqlite3_close(DB);

}

const char * mainwindow::LoadDatabase()
{
    //FIXME  - Separate database logic later here //

    return nullptr;
}
