#include "formdamageinfo.h"
#include "ui_formdamageinfo.h"

#include <QTimer>

FormDamageInfo::FormDamageInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDamageInfo)
{
    ui->setupUi(this);

    m_vValuesDamage.push_back(-2);
    m_vValuesDamage.push_back(-2);

    m_vDamageHistory.push_back(-2);
    m_vDamageHistory.push_back(-2);

    m_vValuesCure.push_back(-2);
    m_vValuesCure.push_back(-2);

    m_vCureHistory.push_back(-2);
    m_vCureHistory.push_back(-2);

    m_vOpacityDamage.push_back(0.0);
    m_vOpacityDamage.push_back(0.0);
    m_vOpacityDamage.push_back(0.0);

    m_vOpacityCure.push_back(0.0);
    m_vOpacityCure.push_back(0.0);
    m_vOpacityCure.push_back(0.0);

    ui->labelDamage0->setStyleSheet("color: rgba(255, 25, 78, 0);");
    ui->labelDamage1->setStyleSheet("color: rgba(255, 25, 78, 0);");
    ui->labelDamage2->setStyleSheet("color: rgba(255, 25, 78, 0);");

    ui->labelCure0->setStyleSheet("color: rgba(0, 22, 0, 0);");
    ui->labelCure1->setStyleSheet("color: rgba(0, 22, 0, 0);");
    ui->labelCure2->setStyleSheet("color: rgba(0, 22, 0, 0);");

    m_ptrTimer = new QTimer(this);
    m_ptrTimer->setSingleShot(true);
    m_ptrTimer->setInterval(50);
    connect(m_ptrTimer, SIGNAL(timeout()), this, SLOT(onTimerOpacityTimeout()));
}

FormDamageInfo::~FormDamageInfo()
{
    delete ui;
}

void FormDamageInfo::pushDamage(int value)
{
    m_vValuesDamage.push_front(value);
    m_vDamageHistory.push_front(m_vValuesDamage.back());
    m_vValuesDamage.pop_back();
    m_vDamageHistory.pop_back();

    m_vOpacityDamage.push_front(1.0);
    m_vOpacityDamage.pop_back();

    if (m_vValuesDamage[0] != -2)
    {
        if (m_vValuesDamage[0] == -1)
            ui->labelDamage0->setText(tr("Miss"));
        else
            ui->labelDamage0->setText(QString::number(m_vValuesDamage[0]));
    }

    if (m_vValuesDamage[1] != -2)
    {
        if (m_vValuesDamage[1] == -1)
            ui->labelDamage1->setText(tr("Miss"));
        else
            ui->labelDamage1->setText(QString::number(m_vValuesDamage[1]));
    }

    QString history;
    for (auto val : m_vDamageHistory)
    {
        if (val != -2)
        {
            if (val == -1)
                history = tr("Miss") + " " + history;
            else
                history = QString::number(val) + " " + history;
        }

    }

    ui->labelDamage2->setText(history);

    m_ptrTimer->start();
}

void FormDamageInfo::pushCure(int value)
{
    m_vValuesCure.push_front(value);
    m_vCureHistory.push_front(m_vValuesCure.back());
    m_vValuesCure.pop_back();
    m_vCureHistory.pop_back();


    m_vOpacityCure.push_front(1.0);
    m_vOpacityCure.pop_back();

    if (m_vValuesCure[0] != -2)
    {
        if (m_vValuesCure[0] == -1)
            ui->labelCure0->setText(tr("Miss"));
        else
            ui->labelCure0->setText(QString::number(m_vValuesCure[0]));
    }

    if (m_vValuesCure[1] != -2)
    {
        if (m_vValuesCure[1] == -1)
            ui->labelCure1->setText(tr("Miss"));
        else
            ui->labelCure1->setText(QString::number(m_vValuesCure[1]));
    }

    QString history;
    for (auto val : m_vCureHistory)
    {
        if (val != -2)
        {
            if (val == -1)
                history = tr("Miss") + " " + history;
            else
                history = QString::number(val) + " " + history;
        }

    }

    ui->labelCure2->setText(history);

    m_ptrTimer->start();
}

void FormDamageInfo::onTimerOpacityTimeout()
{
    bool stop = true;

    double val = 0.01;
    int index = 0;

    for (auto& opacity : m_vOpacityDamage)
    {
        opacity -= val;

        if (opacity <= 0.0)
        {
            if (index == 0)
                m_vValuesDamage[0] = -2;
            else if (index == 1)
                m_vValuesDamage[1] = -2;
            else
                for (auto val : m_vDamageHistory)
                    val = -2;

            opacity = 0.0;
        }
        else
            stop = false;

        index++;
        val += 0.01;
    }

    val = 0.02;
    index = 0;

    for (auto& opacity : m_vOpacityCure)
    {
        opacity -= val;

        if (opacity <= 0.0)
        {
            if (index == 0)
                m_vValuesCure[0] = -2;
            else if (index == 1)
                m_vValuesCure[1] = -2;
            else
                for (auto val : m_vCureHistory)
                    val = -2;

            opacity = 0.0;
        }
        else
            stop = false;

        index++;
        val += 0.01;
    }

    ui->labelDamage0->setStyleSheet(QString("color: rgba(255, 25, 78, %1);").arg(m_vOpacityDamage[0]*255));
    ui->labelDamage1->setStyleSheet(QString("color: rgba(255, 25, 78, %1);").arg(m_vOpacityDamage[1]*255));
    ui->labelDamage2->setStyleSheet(QString("color: rgba(255, 25, 78, %1);").arg(m_vOpacityDamage[2]*255));

    ui->labelCure0->setStyleSheet(QString("color: rgba(0, 22, 0, %1);").arg(m_vOpacityCure[0]*255));
    ui->labelCure1->setStyleSheet(QString("color: rgba(0, 22, 0, %1);").arg(m_vOpacityCure[1]*255));
    ui->labelCure2->setStyleSheet(QString("color: rgba(0, 22, 0, %1);").arg(m_vOpacityCure[2]*255));

    if (!stop)
        m_ptrTimer->start();
}
