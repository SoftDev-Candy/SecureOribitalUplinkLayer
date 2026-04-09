//
// Created by Candy on 4/5/26.
//

#ifndef SOUL_DATABASE_HPP
#define SOUL_DATABASE_HPP
#include <boost/mp11/list.hpp>

#include "../Common/TelemetryFrame.hpp"
#include "../external/sqlite/sqlite3.h"

class Database
{
private:

public:
    static int Database_init(); // Initialize database or open it basically for changing of data//
    static int Create_DB();
    static int CreateTable();
    static void Terminate();//Closes database and will terminate any processes that need to be terminated
    static const int InsertTelemetry(const TelemetryFrame& tframe , uint64_t received_ms) ;//Add data to the

    //FIXME -- I need to do something about keeping this static not a good solution making it static or global
    static sqlite3* DB; // Database connection obj
};

#endif //SOUL_DATABASE_HPP
