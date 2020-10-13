#include "datetime.h"
#include "../global.h"

#include <QDateTime>

using namespace CORE;


static const char* TIME_DATE_MONTHS[] = {
    "Hammer",              // Gennaio
    "Alturiak",            // Febbraio
    "Ches",                // Marzo
    "Tarsakh",             // Aprile
    "Mirtul",              // Maggio
    "Kythorn",             // Giugno
    "Flamerule",           // Luglio
    "Eleasias",            // Agosto
    "Eleint",              // Settembre
    "Marphenot",           // Ottobre
    "Ukthar",              // Novembre
    "Nightal"              // Dicembre
};

static const char* TIME_DATE_MONTHS_ALTERNATIVE_NAME[] = {
    "Profondo inverno",
    "L'artiglio dell'inverno",
    "L'artiglio dei tramonti",
    "L'artiglio delle tempeste",
    "Lo scioglimento",
    "Il tempo dei fiori",
    "Marea d'estate",
    "Solealto",
    "La dissolvenza",
    "Foglie cadute",
    "La decomposizione",
    "L'abbassamento"
};

static const int TIME_DATE_MONTHS_DAYS[] = {
    31, // Mezzinverno
    30,
    30,
    31, // Pratoverde
    30,
    30,
    31, // Mezzestate
    30,
    31, // Granraccolto
    30,
    31, // Festa della Luna
    30
};

static const int TIME_DATE_MONTHS_DAYS_BISSEXTILE[] = {
    31, // Mezzinverno
    30,
    30,
    31, // Pratoverde
    30,
    30,
    32, // Mezzestate, Scudiuniti
    30,
    31, // Granraccolto
    30,
    31, // Festa della Luna
    30
};


DateTime::DateTime()
{

}

QString DateTime::currentDateTimeToString()
{
    QDateTime dateTime = currentDateTime();

    auto years = dateTime.date().year();
    auto daysInYear = dateTime.date().dayOfYear();
    bool isBissextile = dateTime.date().isLeapYear(years);
    int month = 0;

    for (int i = 0; i < 12; ++i)
    {
        if (isBissextile)
        {
            if (TIME_DATE_MONTHS_DAYS_BISSEXTILE[i] > daysInYear)
                daysInYear -= TIME_DATE_MONTHS_DAYS_BISSEXTILE[i];
            else
            {
                month = i;
                break;
            }
        }
        else
        {
            if (TIME_DATE_MONTHS_DAYS[i] > daysInYear)
                daysInYear -= TIME_DATE_MONTHS_DAYS[i];
            else
            {
                month = i;
                break;
            }
        }
    }

    auto days = daysInYear;

    auto hours = dateTime.time().hour();
    auto minutes = dateTime.time().minute();

    QString timeStr = QString("Giorno %1 del mese di %2 (%3), anno %4 della quarta era.")
            .arg(days)
            .arg(TIME_DATE_MONTHS[month])
            .arg(TIME_DATE_MONTHS_ALTERNATIVE_NAME[month])
            .arg(years) + QMUD_GLOBAL_NEW_LINE;

    if (month == 0 && days == 31)
        timeStr += QString("E' il giorno di Mezz'inverno");
    if (month == 3 && days == 31)
        timeStr += QString("E' il giorno di Prato Verde");
    if (month == 6 && days == 31)
        timeStr += QString("E' il giorno di Mezz'estate");
    if (month == 6 && days == 32)
        timeStr += QString("E' il giorno di Scudi Uniti");
    if (month == 8 && days == 31)
        timeStr += QString("E' il giorno di Gran Raccolto");
    if (month == 10 && days == 31)
        timeStr += QString("E' la Festa della Luna");

    timeStr += QString("Sono le ore %1 e %2").arg(hours).arg(minutes);

    return timeStr;
}

bool DateTime::isDark()
{
    auto currentTime = currentDateTime();
    return currentTime.time().hour() > 22 && currentTime.time().hour() < 5;
}

bool DateTime::isAlmostDark()
{
    auto currentTime = currentDateTime();
    return currentTime.time().hour() > 19 && currentTime.time().hour() < 7;
}

QDateTime DateTime::currentDateTime()
{
    auto msFromEpoch = (QDateTime::currentMSecsSinceEpoch() - (static_cast<qint64>(50)*365*24*60*60*1000) - static_cast<qint64>(200)*24*60*60*1000) * 12;

    return QDateTime::fromMSecsSinceEpoch(msFromEpoch);
}
