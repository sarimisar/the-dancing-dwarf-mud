/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef GLOBALDEBUG_H
#define GLOBALDEBUG_H

#include <QDebug>

#define GLOBAL_DEBUG(__str) qDebug().noquote() << __str
#define GLOBAL_INFO(__str) qInfo().noquote() << __str
#define GLOBAL_WARNING(__str) qWarning().noquote() << __str
#define GLOBAL_CRITICAL(__str) qCritical().noquote() << __str
#define GLOBAL_BUG(__str) qCritical().noquote() << "[BUG]" << __str
#define GLOBAL_CRITICAL_JSON_OBJ(__obj) qCritical().noquote() << QObject::tr("Oggetto Radice") << " ->" << QJsonDocument(__obj).toJson(QJsonDocument::Indented)

#define GLOBAL_DEBUG_DESTRUCTOR(__class, __str) GLOBAL_DEBUG(QString("[DEBUG_DESTRUCTOR] Class %1: %2").arg(__class).arg(QString(__str).isEmpty() ? "<none>" : __str))
//#define GLOBAL_DEBUG_DESTRUCTOR(__class, __str) (void)0


#endif // GLOBALDEBUG_H
