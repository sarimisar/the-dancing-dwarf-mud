#ifndef FORMCREATECHARACTER_H
#define FORMCREATECHARACTER_H

#include <QWidget>

#include "../common.h"

namespace Ui {
class FormCreateCharacter;
}

class FormCreateCharacter : public QWidget
{
    Q_OBJECT

public:
    explicit FormCreateCharacter(QWidget *parent = nullptr);
    ~FormCreateCharacter();

public slots:
    void onMessage(QMUD::ClientDataMessage::Message message);

signals:
    void sendCommand(QString cmd);

private slots:
    void on_pushButtonClassWarrior_toggled(bool checked);
    void on_pushButtonClassMage_toggled(bool checked);
    void on_pushButtonClassBarbarian_toggled(bool checked);
    void on_spinBoxInt_valueChanged(int arg1);
    void on_spinBoxWis_valueChanged(int arg1);
    void on_spinBoxStr_valueChanged(int arg1);
    void on_spinBoxCon_valueChanged(int arg1);
    void on_spinBoxDex_valueChanged(int arg1);
    void on_comboBoxRace_currentIndexChanged(int index);
    void on_lineEditName_textChanged(const QString &arg1);
    void on_pushButtonCreate_clicked();
    void on_pushButtonCancel_clicked();

private:
    void resetStatistics();
    void updateStatistics();
    void updateCreateButton();

private:
    Ui::FormCreateCharacter *ui;

    int m_iStatPoints;
};

#endif // FORMCREATECHARACTER_H
