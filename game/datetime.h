#ifndef DATETIME_H
#define DATETIME_H

#include <QString>
#include <QDateTime>


namespace CORE
{

class DateTime
{
public:
    DateTime();

    static QString currentDateTimeToString();
    static bool isDark();
    static bool isAlmostDark();

private:
    static QDateTime currentDateTime();
};

}

#endif // DATETIME_H
