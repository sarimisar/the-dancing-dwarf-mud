#include "formcharacterinfo.h"
#include "ui_formcharacterinfo.h"

FormCharacterInfo::FormCharacterInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCharacterInfo)
{
    ui->setupUi(this);
}

FormCharacterInfo::~FormCharacterInfo()
{
    delete ui;
}

void FormCharacterInfo::setData(QMUD::PcCharacterBaseInfo &data)
{
    m_data = data;

    ui->labelTargetLevel->setText(QString::number(data.level()));
    ui->labelTargetName->setText(data.name().at(0).toUpper() + data.name().mid(1));
    ui->labelTargetRaceAndClass->setText(QString("%1 - %2").arg(QMUD::Race::toReadableString(data.race())).arg(QMUD::classTypeToReadableString(data.classType())));
}

QString FormCharacterInfo::name() const
{
    return m_data.name();
}

void FormCharacterInfo::on_pushButtonConnect_clicked()
{
    emit connect(m_data.name());
}
