//
// Created by Candy on 4/5/26.
//

#include "Database.hpp"

#include <iostream>

int Database::database_init()
{
    sqlite3* DB; // Database connection obj

    int rc = sqlite3_open("Soul.db",&DB);//Call the sqlite constructor

    if (rc != SQLITE_OK)
    {
        std::cerr<<"Failed to open database ERROR  : "<<sqlite3_errmsg(DB)<<"\n";
        sqlite3_close(DB);
        return 1;
    }

    //Create SQL table named telemetry if it doesn't exist already//

const char* sql =   "CREATE TABLE IF NOT EXISTS Telemetry("
                    "id                 INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "Satellite_name     TEXT NOT NULL, "
                    "timestamp_ms       INTEGER NOT NULL, "
                    "battery            REAL NOT NULL, "
                    "temperature        REAL NOT NULL, "
                    "received_ms        INT NOT NULL );";

    //Apparently this code cant take strings so we use only chars now
     char* error_msg =  nullptr;
    //call exec on the database and the sql
    rc = sqlite3_exec(DB,sql,nullptr,nullptr,&error_msg);
    if (rc != SQLITE_OK)
    {
        std::cerr<<"Failed to open database ERROR  : "<<error_msg<<"\n";
        sqlite3_close(DB);
        return 1;
    }

    sqlite3_close(DB);
    return 0;

}
