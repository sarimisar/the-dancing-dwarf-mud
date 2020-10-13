#ifndef FORMDAMAGEINFO_H
#define FORMDAMAGEINFO_H

#include <QWidget>
#include <QPropertyAnimation>

namespace Ui {
class FormDamageInfo;
}

class FormDamageInfo : public QWidget
{
    Q_OBJECT

public:
    explicit FormDamageInfo(QWidget *parent = nullptr);
    ~FormDamageInfo();

    void pushDamage(int value);
    void pushCure(int value);

private slots:
    void onTimerOpacityTimeout();

private:
    Ui::FormDamageInfo *ui;

    QVector<int> m_vValuesDamage;
    QVector<int> m_vValuesCure;

    QVector<int> m_vDamageHistory;
    QVector<int> m_vCureHistory;

    QVector<double> m_vOpacityDamage;
    QVector<double> m_vOpacityCure;

    QTimer* m_ptrTimer;
};

#endif // FORMDAMAGEINFO_H
