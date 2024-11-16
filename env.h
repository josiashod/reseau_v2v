#ifndef ENV_H
#define ENV_H

#include <QString>

namespace env
{
    extern QString DB_CONNECTION;
    extern QString DB_HOST;
    extern unsigned int DB_PORT;
    extern QString DB_DATABASE;
    extern QString DB_USERNAME;
    extern QString DB_PASSWORD;
}

#endif // ENV_H
