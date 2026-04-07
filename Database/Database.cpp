//
// Created by Candy on 4/5/26.
//

#include "Database.hpp"

#include <iostream>
sqlite3* Database::DB;
int Database::Database_init()
{
    Create_DB();
    CreateTable();
    return 0;
}

//FIXME -- ADD CREATE DATABASE LOGIC HERE
int Database::Create_DB()
{
    int rc = sqlite3_open("Soul.db",&DB);//Call the sqlite constructor

    if (rc != SQLITE_OK)
    {
        std::cerr<<"Failed to open database ERROR  : "<<sqlite3_errmsg(DB)<<"\n";
        sqlite3_close(DB);
        return 1;
    }
return 0;
}

//FIXME -- ADD CREATE TABLE LOGIC HERE
int Database::CreateTable()
{    //Create SQL table named telemetry if it doesn't exist already//

    const char* sql =   "CREATE TABLE IF NOT EXISTS Telemetry("
                        "id                 INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "Satellite_name     TEXT NOT NULL, "
                        "timestamp_ms       INTEGER NOT NULL, "
                        "battery            REAL NOT NULL, "
                        "temperature        REAL NOT NULL, "
                        "received_ms        INT NOT NULL );";

    //Apparently this exec function cant take strings so we use only chars now
    //FIXME -- WRAP THIS WHOLE SEGMENT IN A TRY AND CATCH MAYBE //
    char* error_msg =  nullptr;
    //call exec on the database and the SQL
    int rc = sqlite3_exec(DB,sql,nullptr,nullptr,&error_msg);
    if (rc != SQLITE_OK)
    {
        std::cerr<<"Failed to open database ERROR  : "<<error_msg<<"\n";
        sqlite3_close(DB);
        return 1;
    }
    else
    {
        std::cout<<"Database opened/Created and is ready to be accessed"<<std::endl;
    }

    return 0;
}

void Database::Terminate()
{
    sqlite3_close(DB);
    std::cout<<"Database is closed as end of while loop -- Quit condition is applied"<<std::endl;

}

const int Database::InsertTelemetry(const TelemetryFrame &tframe , uint64_t received_ms)
{





return 0;
}



