/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QString>

class ConfigFile
{
public:
    ConfigFile();

    QString dataDirectory() const;
    quint16 port() const;

private:
    void load();

private:
    QString m_strDataDirectory;
    quint16 m_uiPort;
};

#endif // CONFIGFILE_H
