#include "formroomcontent.h"
#include "ui_formroomcontent.h"

#include <QMouseEvent>

FormRoomContent::FormRoomContent(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRoomContent),
    m_iNpcId(QMUD::IdInvalid)
{
    ui->setupUi(this);

    for (auto obj : children())
        obj->installEventFilter(this);
}

FormRoomContent::~FormRoomContent()
{
    delete ui;
}

void FormRoomContent::setNpcData(QString name, int level, int hp, int hpMax, int mp, int mpMax, QMUD::IdType id)
{
    ui->widgetNpc->setVisible(true);
    ui->labelNpcName->setText(name);
    ui->labelNpcLevel->setText(QString::number(level));
    ui->progressBarNpcHp->setMaximum(hpMax);
    ui->progressBarNpcHp->setValue(hp);
    ui->progressBarNpcMp->setMaximum(mpMax);
    ui->progressBarNpcMp->setValue(mp);

    m_iNpcId = id;
}

bool FormRoomContent::isNpc() const
{
    return ui->widgetNpc->isVisible();
}

QMUD::IdType FormRoomContent::npcId() const
{
    return m_iNpcId;
}

bool FormRoomContent::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        emit clicked();
        return true;
    }

    return QObject::eventFilter(watched, event);
}
