/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/
#include "color.h"
#include "../global.h"

using namespace CORE;

/* These are the ANSI codes for foreground text colors */
#define ANSI_BLACK   "\033[0;30m"
#define ANSI_DRED    "\033[0;31m"
#define ANSI_DGREEN  "\033[0;32m"
#define ANSI_ORANGE  "\033[0;33m"
#define ANSI_DBLUE   "\033[0;34m"
#define ANSI_PURPLE  "\033[0;35m"
#define ANSI_CYAN    "\033[0;36m"
#define ANSI_GREY    "\033[0;37m"
#define ANSI_DGREY   "\033[1;30m"
#define ANSI_RED     "\033[1;31m"
#define ANSI_GREEN   "\033[1;32m"
#define ANSI_YELLOW  "\033[1;33m"
#define ANSI_BLUE    "\033[1;34m"
#define ANSI_PINK    "\033[1;35m"
#define ANSI_LBLUE   "\033[1;36m"
#define ANSI_WHITE   "\033[1;37m"

/* These are the ANSI codes for background colors */
#define BACK_BLACK   "\033[40m"
#define BACK_DRED    "\033[41m"
#define BACK_DGREEN  "\033[42m"
#define BACK_ORANGE  "\033[43m"
#define BACK_DBLUE   "\033[44m"
#define BACK_PURPLE  "\033[45m"
#define BACK_CYAN    "\033[46m"
#define BACK_GREY    "\033[47m"

/* Other miscelaneous ANSI tags that can be used */
#define ANSI_RESET      "\033[0m"   /* Reset to terminal default */
#define ANSI_BOLD       "\033[1m"   /* For bright color stuff */
#define ANSI_ITALIC     "\033[3m"   /* Italic text */
#define ANSI_UNDERLINE  "\033[4m"   /* Underline text */
#define ANSI_REVERSE    "\033[7m"   /* Reverse colors */
#define ANSI_STRIKEOUT  "\033[9m"   /* Overstrike line */




QString Color::strEscapeColorsConversion(QString src, bool colored, bool customClient)
{
    QString dst;

    for (int i = 0; i < src.length(); ++i)
    {
        switch (src[i].unicode())
        {
        case '%':
            if (i+1 >= src.length())
            {
                // Stringa di input non ben formata
                dst += "%";
                return dst;
            }

            switch (src[i+1].unicode())
            {
            case '%':  /* Escaped self, return one of us */
                dst += "%";
                ++i;
                break;

            case '[':
                {
                    int c = i+2;
                    while (src[c] != ']' && c < src.length())
                        ++c;

                    // ] not found
                    if (src[c] != ']')
                    {
                        dst += "[";
                        break;
                    }

                    QString command = src.mid(i+2, c-(i+2));
                    i = c;
                    if (customClient)
                        dst += "\034[" + command + "]";
                }
                break;
            }
            break;

        case '&':  /* NORMAL, Foreground colour */
            if (i+1 >= src.length())
            {
                // Stringa di input non ben formata
                dst += "&";
                return dst;
            }

            switch (src[i+1].unicode())
            {
            case '&':  /* Escaped self, return one of us */
                dst += "&";
                ++i;
                break;

            case '[':  /* Symbolic color name */
                {
                    int c = i+2;
                    while (src[c] != ']' && c < src.length())
                        ++c;

                    // ] not found
                    if (src[c] != ']')
                    {
                        dst += "[";
                        break;
                    }

                    QString colorCode = src.mid(i+2, c-(i+2));
                    i = c;
                    dst += "\033[38;5;" + colorCode + "m";
                }
                break;

            case 'i':  /* Italic text */
            case 'I':
                if (colored)
                    dst += ANSI_ITALIC;
                ++i;
                break;

            case 'v':  /* Reverse colors */
            case 'V':
                if (colored)
                    dst += ANSI_REVERSE;
                ++i;
                break;

            case 'a':  /* Bold text */
            case 'A':
                if (colored)
                    dst += ANSI_BOLD;
                ++i;
                break;

            case 'u':  /* Underline */
            case 'U':
                if (colored)
                    dst += ANSI_UNDERLINE;
                ++i;
                break;

            case 's':  /* Strikeover */
            case 'S':
                if (colored)
                    dst += ANSI_STRIKEOUT;
                ++i;
                break;

            case 'd':  /* Player's client default color */
            case 'D':
                if (colored)
                    dst += ANSI_RESET;
                ++i;
                break;

            case 'x':  /* Black */
                if (colored)
                    dst += ANSI_BLACK;
                ++i;
                break;

            case 'O':  /* Orange/Brown */
                if (colored)
                    dst += ANSI_ORANGE;
                ++i;
                break;

            case 'c':  /* Cyan */
                if (colored)
                    dst += ANSI_CYAN;
                ++i;
                break;

            case 'z':  /* Dark Grey */
                if (colored)
                    dst += ANSI_DGREY;
                ++i;
                break;

            case 'g':  /* Dark Green */
                if (colored)
                    dst += ANSI_DGREEN;
                ++i;
                break;

            case 'G':  /* Light Green */
                if (colored)
                    dst += ANSI_GREEN;
                ++i;
                break;

            case 'P':  /* Pink/Light Purple */
                if (colored)
                    dst += ANSI_PINK;
                ++i;
                break;

            case 'r':  /* Dark Red */
                if (colored)
                    dst += ANSI_DRED;
                ++i;
                break;

            case 'b':  /* Dark Blue */
                if (colored)
                    dst += ANSI_DBLUE;
                ++i;
                break;

            case 'w':  /* Grey */
                if (colored)
                    dst += ANSI_GREY;
                ++i;
                break;

            case 'Y':  /* Yellow */
                if (colored)
                    dst += ANSI_YELLOW;
                ++i;
                break;

            case 'C':  /* Light Blue */
                if (colored)
                    dst += ANSI_LBLUE;
                ++i;
                break;

            case 'p':  /* Purple */
                if (colored)
                    dst += ANSI_PURPLE;
                ++i;
                break;

            case 'R':  /* Red */
                if (colored)
                    dst += ANSI_RED;
                ++i;
                break;

            case 'B':  /* Blue */
                if (colored)
                    dst += ANSI_BLUE;
                ++i;
                break;

            case 'W':  /* White */
                if (colored)
                    dst += ANSI_WHITE;
                ++i;
                break;

            default:   /* Unknown sequence, return all the chars */
                dst += src[i];
                dst += src[i+1];
                ++i;
                break;
            }
            break;

        case '{':  /* BACKGROUND colour */
            if (i+1 >= src.length())
            {
                // Stringa di input non ben formata
                dst += "{";
                return dst;
            }

            switch (src[i+1].unicode())
            {
            case '{':  /* Escaped self, return one of us */
                dst += "{";
                ++i;
                break;

            case '[':  /* Symbolic color name */
                {
                    int c = i+2;
                    while (src[c] != ']' && c < src.length())
                        ++c;

                    // ] not found
                    if (src[c] != ']')
                    {
                        dst += "[";
                        break;
                    }

                    QString colorCode = src.mid(i+2, c-(i+2));
                    i = c;
                    dst += "\033[48;5;" + colorCode + "m";
                }
                break;

            case 'x':  /* Black */
                if (colored)
                    dst += BACK_BLACK;
                ++i;
                break;

            case 'r':  /* Dark Red */
                if (colored)
                    dst += BACK_DRED;
                ++i;
                break;

            case 'g':  /* Dark Green */
                if (colored)
                    dst += BACK_DGREEN;
                ++i;
                break;

            case 'O':  /* Orange/Brown */
                if (colored)
                    dst += BACK_ORANGE;
                ++i;
                break;

            case 'b':  /* Dark Blue */
                if (colored)
                    dst += BACK_DBLUE;
                ++i;
                break;

            case 'p':  /* Purple */
                if (colored)
                    dst += BACK_PURPLE;
                ++i;
                break;

            case 'c':  /* Cyan */
                if (colored)
                    dst += BACK_CYAN;
                ++i;
                break;

            case 'w':  /* Grey */
                if (colored)
                    dst += BACK_GREY;
                ++i;
                break;

            default:   /* Unknown sequence, return all the chars */
                dst += src[i];
                dst += src[i+1];
                ++i;
                break;
            }
            break;

        default:   /* Just a normal character */
            dst += src[i];
            break;
        }
    }

    return dst;
}

QString Color::strTestColor()
{
    QString dst;

    dst += "-------------------- ANSI COLORS --------------------" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&xBlack" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&OOrange/Brown" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&cCyan" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&zDark Grey" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&gDark Green" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&GLight Green" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&PPink/Light Purple" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&rDark Red" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&bDark Blue" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&wGrey" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&YYellow" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&CLight Blue" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&pPurple" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&RRed" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&BBlue" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "&WWhite" + QString(QMUD_GLOBAL_NEW_LINE);
    dst += "-------------------- XTERM COLORS --------------------" + QString(QMUD_GLOBAL_NEW_LINE);

    for (int y=0; y<32; y++)
    {
        for (int x=0; x<8; x++)
            dst += QString("{[%1] %2 &d ").arg(y*8 + x).arg(QString::number(y*8 + x), 3);

        dst += QMUD_GLOBAL_NEW_LINE;
    }

    return dst;
}
