/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef EDITEXITDIALOG_H
#define EDITEXITDIALOG_H

#include "../common.h"

#include <QDialog>
#include <QUuid>

namespace Ui {
class EditExitDialog;
}

class EditorMap;

class EditExitDialog : public QDialog
{
    Q_OBJECT

public:
    struct ExitData
    {
        ExitData() :
            code(QUuid::createUuid().toString()),
            isHidden(false),
            isOpen(false),
            action(QMUD::ActionType::UNKNOWN)
        { }

        ExitData(const ExitData& other)
        {
            code           = other.code;
            door           = other.door;
            isHidden       = other.isHidden;
            isOpen         = other.isOpen;
            descOpen       = other.descOpen;
            descOpenOther  = other.descOpenOther;
            descClose      = other.descClose;
            descCloseOther = other.descCloseOther;
            action         = other.action;
        }

        void operator=(const ExitData& other)
        {
            code           = other.code;
            door           = other.door;
            isHidden       = other.isHidden;
            isOpen         = other.isOpen;
            descOpen       = other.descOpen;
            descOpenOther  = other.descOpenOther;
            descClose      = other.descClose;
            descCloseOther = other.descCloseOther;
            action         = other.action;
        }

        void serialize(QJsonObject& obj) const
        {
            obj["code"] = code;
            obj["door"] = door;
            obj["isHidden"] = isHidden;
            obj["isOpen"] = isOpen;
            obj["descOpen"] = descOpen;
            obj["descOpenOther"] = descOpenOther;
            obj["descClose"] = descClose;
            obj["descCloseOther"] = descCloseOther;
            obj["action"] = QMUD::actionToString(action);
        }

        void deserialize(const QJsonObject& obj)
        {
            code = obj["code"].toString();
            door = obj["door"].toString();
            isHidden = obj["isHidden"].toBool();
            isOpen = obj["isOpen"].toBool();
            descOpen = obj["descOpen"].toString();
            descOpenOther = obj["descOpenOther"].toString();
            descClose = obj["descClose"].toString();
            descCloseOther = obj["descCloseOther"].toString();
            action = QMUD::actionFromString(obj["action"].toString());
        }

        QString code;
        QString door;
        bool isHidden;
        bool isOpen;
        QString descOpen;
        QString descOpenOther;
        QString descClose;
        QString descCloseOther;
        QMUD::ActionType action;
    };

    explicit EditExitDialog(QWidget *parent = nullptr);
    ~EditExitDialog();

    void setData(const ExitData& data);
    ExitData data() const;

private:
    Ui::EditExitDialog *ui;
};

Q_DECLARE_METATYPE(EditExitDialog::ExitData)

#endif // EDITEXITDIALOG_H
