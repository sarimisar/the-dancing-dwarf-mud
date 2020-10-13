/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef COLOR_H
#define COLOR_H

#include <QString>

namespace CORE
{

class Color
{
public:
    static QString strEscapeColorsConversion(QString str, bool colored, bool customClient);
    static QString strTestColor();

private:
    Color() { }
};

}

#endif // COLOR_H
