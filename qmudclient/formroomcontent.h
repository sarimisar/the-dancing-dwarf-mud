#ifndef FORMROOMCONTENT_H
#define FORMROOMCONTENT_H

#include "../global.h"

#include <QWidget>

namespace Ui {
class FormRoomContent;
}

class FormRoomContent : public QWidget
{
    Q_OBJECT

public:
    explicit FormRoomContent(QWidget *parent = nullptr);
    ~FormRoomContent();

    void setNpcData(QString name, int level, int hp, int hpMax, int mp, int mpMax, QMUD::IdType id);

    bool isNpc() const;
    QMUD::IdType npcId() const;

signals:
    void clicked();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::FormRoomContent *ui;

    QMUD::IdType m_iNpcId;
};

#endif // FORMROOMCONTENT_H
