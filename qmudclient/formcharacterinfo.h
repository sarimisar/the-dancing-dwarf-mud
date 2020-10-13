#ifndef FORMCHARACTERINFO_H
#define FORMCHARACTERINFO_H

#include "../common.h"

#include <QWidget>

namespace Ui {
class FormCharacterInfo;
}

class FormCharacterInfo : public QWidget
{
    Q_OBJECT

public:
    explicit FormCharacterInfo(QWidget *parent = nullptr);
    ~FormCharacterInfo();

    void setData(QMUD::PcCharacterBaseInfo& data);
    QString name() const;

signals:
    void connect(QString name);

private slots:
    void on_pushButtonConnect_clicked();

private:
    Ui::FormCharacterInfo *ui;

    QMUD::PcCharacterBaseInfo m_data;
};

#endif // FORMCHARACTERINFO_H
