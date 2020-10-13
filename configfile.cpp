/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "configfile.h"

#include <QSettings>
#include <QDebug>

ConfigFile::ConfigFile()
{
    load();
}

QString ConfigFile::dataDirectory() const
{
    return m_strDataDirectory;
}

quint16 ConfigFile::port() const
{
    return m_uiPort;
}

void ConfigFile::load()
{
    QSettings settings("qmud_config.ini", QSettings::IniFormat);

    m_strDataDirectory = settings.value("data_directory", "qmudroot").toString();
    m_uiPort = static_cast<quint16>(settings.value("Network/port", 8060).toUInt());
}
