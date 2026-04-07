//
// Created by Candy on 4/5/26.
//

#ifndef SOUL_DATABASE_HPP
#define SOUL_DATABASE_HPP
#include <boost/mp11/list.hpp>

#include "../external/sqlite/sqlite3.h"

class Database
{
private:


public:

    static int Database_init(); // Initialize database or open it basically for changing of data//
    static void Terminate();
    static sqlite3* DB; // Database connection obj


};



#endif //SOUL_DATABASE_HPP
