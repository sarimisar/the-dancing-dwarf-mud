#include "formcreatecharacter.h"
#include "ui_formcreatecharacter.h"

#include "../common.h"

FormCreateCharacter::FormCreateCharacter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCreateCharacter),
    m_iStatPoints(0)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    resetStatistics();

    ui->labelErrorNameNotValid->setVisible(false);
    ui->pushButtonCreate->setEnabled(false);
}

FormCreateCharacter::~FormCreateCharacter()
{
    delete ui;
}

void FormCreateCharacter::onMessage(QMUD::ClientDataMessage::Message message)
{
    if (message == QMUD::ClientDataMessage::Message::ERROR_CH_NAME_FAILED)
        ui->labelErrorNameNotValid->setVisible(true);
    else if (message == QMUD::ClientDataMessage::Message::INFO_CH_CREATION_OK)
        close();
}

void FormCreateCharacter::resetStatistics()
{
    QMUD::ClassType chClass = QMUD::ClassType::WARRIOR;

    if (ui->pushButtonClassMage->isChecked())
        chClass = QMUD::ClassType::MAGE;
    else if (ui->pushButtonClassBarbarian->isChecked())
        chClass = QMUD::ClassType::BARBARIAN;

    QMUD::RaceType race = QMUD::RaceType::HUMAN;

    if (ui->comboBoxRace->currentIndex() == 1)
        race = QMUD::RaceType::DWARF;
    else if (ui->comboBoxRace->currentIndex() == 2)
        race = QMUD::RaceType::GNOME;
    else if (ui->comboBoxRace->currentIndex() == 3)
        race = QMUD::RaceType::HALF_ORC;
    else if (ui->comboBoxRace->currentIndex() == 4)
        race = QMUD::RaceType::HIGH_ELF;
    else if (ui->comboBoxRace->currentIndex() == 5)
        race = QMUD::RaceType::HALF_ELF;

    int maxInt = 18;
    int maxWis = 18;
    int maxStr = 18;
    int maxCon = 18;
    int maxDex = 18;
    int minInt = QMUD::Race::basePCInt(race);
    int minWis = QMUD::Race::basePCWis(race);
    int minStr = QMUD::Race::basePCCon(race);
    int minCon = QMUD::Race::basePCStr(race);
    int minDex = QMUD::Race::basePCDex(race);

    QMUD::classTypeToMaxStatistics(chClass, maxInt, maxWis, maxStr, maxCon, maxDex);

    ui->spinBoxInt->setRange(minInt, maxInt);
    ui->spinBoxWis->setRange(minWis, maxWis);
    ui->spinBoxStr->setRange(minStr, maxStr);
    ui->spinBoxCon->setRange(minCon, maxCon);
    ui->spinBoxDex->setRange(minDex, maxDex);

    ui->spinBoxInt->blockSignals(true);
    ui->spinBoxWis->blockSignals(true);
    ui->spinBoxStr->blockSignals(true);
    ui->spinBoxCon->blockSignals(true);
    ui->spinBoxDex->blockSignals(true);

    ui->spinBoxInt->setValue(minInt);
    ui->spinBoxWis->setValue(minWis);
    ui->spinBoxStr->setValue(minStr);
    ui->spinBoxCon->setValue(minCon);
    ui->spinBoxDex->setValue(minDex);

    ui->spinBoxInt->blockSignals(false);
    ui->spinBoxWis->blockSignals(false);
    ui->spinBoxStr->blockSignals(false);
    ui->spinBoxCon->blockSignals(false);
    ui->spinBoxDex->blockSignals(false);

    ui->labelStatisticsPoints->setText(tr("%1 punti rimanenti").arg(QMUD_GLOBAL_CH_NUMBER_OF_STATISTICS_POINT));

    m_iStatPoints = 0;

    updateCreateButton();
}

void FormCreateCharacter::updateStatistics()
{
    QMUD::ClassType chClass = QMUD::ClassType::WARRIOR;

    if (ui->pushButtonClassMage->isChecked())
        chClass = QMUD::ClassType::MAGE;
    else if (ui->pushButtonClassBarbarian->isChecked())
        chClass = QMUD::ClassType::BARBARIAN;

    QMUD::RaceType race = QMUD::RaceType::HUMAN;

    if (ui->comboBoxRace->currentIndex() == 1)
        race = QMUD::RaceType::DWARF;
    else if (ui->comboBoxRace->currentIndex() == 2)
        race = QMUD::RaceType::GNOME;
    else if (ui->comboBoxRace->currentIndex() == 3)
        race = QMUD::RaceType::HALF_ORC;
    else if (ui->comboBoxRace->currentIndex() == 4)
        race = QMUD::RaceType::HIGH_ELF;
    else if (ui->comboBoxRace->currentIndex() == 5)
        race = QMUD::RaceType::HALF_ELF;

    int maxInt = 18;
    int maxWis = 18;
    int maxStr = 18;
    int maxCon = 18;
    int maxDex = 18;
    int minInt = QMUD::Race::basePCInt(race);
    int minWis = QMUD::Race::basePCWis(race);
    int minStr = QMUD::Race::basePCCon(race);
    int minCon = QMUD::Race::basePCStr(race);
    int minDex = QMUD::Race::basePCDex(race);

    QMUD::classTypeToMaxStatistics(chClass, maxInt, maxWis, maxStr, maxCon, maxDex);

    int totalPoints =
            (ui->spinBoxInt->value() - minInt) +
            (ui->spinBoxWis->value() - minWis) +
            (ui->spinBoxStr->value() - minStr) +
            (ui->spinBoxCon->value() - minCon) +
            (ui->spinBoxDex->value() - minDex);

    if (totalPoints >= QMUD_GLOBAL_CH_NUMBER_OF_STATISTICS_POINT)
    {
        ui->spinBoxInt->setMaximum(ui->spinBoxInt->value());
        ui->spinBoxWis->setMaximum(ui->spinBoxWis->value());
        ui->spinBoxStr->setMaximum(ui->spinBoxStr->value());
        ui->spinBoxCon->setMaximum(ui->spinBoxCon->value());
        ui->spinBoxDex->setMaximum(ui->spinBoxDex->value());
    }
    else
    {
        ui->spinBoxInt->setMaximum(maxInt);
        ui->spinBoxWis->setMaximum(maxWis);
        ui->spinBoxStr->setMaximum(maxStr);
        ui->spinBoxCon->setMaximum(maxCon);
        ui->spinBoxDex->setMaximum(maxDex);
    }

    ui->labelStatisticsPoints->setText(tr("%1 punti rimanenti").arg(QMUD_GLOBAL_CH_NUMBER_OF_STATISTICS_POINT - totalPoints));

    m_iStatPoints = totalPoints;

    updateCreateButton();
}

void FormCreateCharacter::updateCreateButton()
{
    bool enabled = true;

    if (ui->lineEditName->text().size() < 3)
        enabled = false;

    if (m_iStatPoints != QMUD_GLOBAL_CH_NUMBER_OF_STATISTICS_POINT)
        enabled = false;

    ui->pushButtonCreate->setEnabled(enabled);
}

void FormCreateCharacter::on_pushButtonClassWarrior_toggled(bool checked)
{
    if (checked)
        resetStatistics();
}

void FormCreateCharacter::on_pushButtonClassMage_toggled(bool checked)
{
    if (checked)
        resetStatistics();
}

void FormCreateCharacter::on_pushButtonClassBarbarian_toggled(bool checked)
{
    if (checked)
        resetStatistics();
}

void FormCreateCharacter::on_spinBoxInt_valueChanged(int arg1)
{
    updateStatistics();
}

void FormCreateCharacter::on_spinBoxWis_valueChanged(int arg1)
{
    updateStatistics();
}

void FormCreateCharacter::on_spinBoxStr_valueChanged(int arg1)
{
    updateStatistics();
}

void FormCreateCharacter::on_spinBoxCon_valueChanged(int arg1)
{
    updateStatistics();
}

void FormCreateCharacter::on_spinBoxDex_valueChanged(int arg1)
{
    updateStatistics();
}

void FormCreateCharacter::on_comboBoxRace_currentIndexChanged(int index)
{
    resetStatistics();
}

void FormCreateCharacter::on_lineEditName_textChanged(const QString &arg1)
{
    updateCreateButton();
}

void FormCreateCharacter::on_pushButtonCreate_clicked()
{
    ui->labelErrorNameNotValid->setVisible(false);

    QMUD::ClassType chClass = QMUD::ClassType::WARRIOR;

    if (ui->pushButtonClassMage->isChecked())
        chClass = QMUD::ClassType::MAGE;
    else if (ui->pushButtonClassBarbarian->isChecked())
        chClass = QMUD::ClassType::BARBARIAN;

    QMUD::RaceType race = QMUD::RaceType::HUMAN;

    if (ui->comboBoxRace->currentIndex() == 1)
        race = QMUD::RaceType::DWARF;
    else if (ui->comboBoxRace->currentIndex() == 2)
        race = QMUD::RaceType::GNOME;
    else if (ui->comboBoxRace->currentIndex() == 3)
        race = QMUD::RaceType::HALF_ORC;
    else if (ui->comboBoxRace->currentIndex() == 4)
        race = QMUD::RaceType::HIGH_ELF;
    else if (ui->comboBoxRace->currentIndex() == 5)
        race = QMUD::RaceType::HALF_ELF;

    QMUD::SexType sex = QMUD::SexType::MALE;

    if (ui->comboBoxSex->currentIndex() == 1)
        sex = QMUD::SexType::FEMALE;

    sendCommand(QString("ch_create %1 %2 %3 %4 %5 %6 %7 %8 %9")
                .arg(ui->lineEditName->text())
                .arg(QMUD::classTypeToString(chClass))
                .arg(QMUD::Race::toString(race))
                .arg(QMUD::sexTypeToString(sex))
                .arg(ui->spinBoxInt->value())
                .arg(ui->spinBoxWis->value())
                .arg(ui->spinBoxCon->value())
                .arg(ui->spinBoxStr->value())
                .arg(ui->spinBoxDex->value()));
}

void FormCreateCharacter::on_pushButtonCancel_clicked()
{
    close();
}
