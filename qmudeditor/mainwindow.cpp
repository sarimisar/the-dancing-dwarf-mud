/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editormap.h"
#include "editornpc.h"
#include "editoritem.h"
#include "formcenteredcheckbox.h"
#include "editexitdialog.h"
#include "scripteditordialog.h"
#include "treewidgetaction.h"
#include "editquestdialog.h"
#include "formtilesmapeditor.h"

#include <QTableWidgetItem>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QMenu>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_mapOnInterface(Q_NULLPTR),
    m_widgetMapOnInterface(Q_NULLPTR),
    m_npcOnInterface(Q_NULLPTR),
    m_widgetNpcOnInterface(Q_NULLPTR),
    m_itemOnInterface(Q_NULLPTR),
    m_widgetItemOnInterface(Q_NULLPTR),
    m_ptrMapEditor(Q_NULLPTR)
{
    ui->setupUi(this);

    ui->treeWidgetMaps->setColumnCount(1);
    ui->stackedWidgetMain->setCurrentWidget(ui->pageEmpty);
    ui->widgetMaps->setMaximumWidth(ui->widgetMaps->maximumHeight());
    //ui->tableWidgetNpcLoot->setColumnCount(2);

    connect(ui->pushButtonNewHome, SIGNAL(clicked()), this, SLOT(on_pushButtonNew_clicked()));
    connect(ui->pushButtonOpenHome, SIGNAL(clicked()), this, SLOT(on_pushButtonOpen_clicked()));

    ui->stackedWidget->setCurrentIndex(1);

    // Npc flags
    ui->tableWidgetNpcFlags->setRowCount(7);

    ui->tableWidgetNpcFlags->setItem(0, 1, new QTableWidgetItem(QObject::tr("IA COMBATTIMENTO - Intelligenza artificiale nei combattimenti")));
    ui->tableWidgetNpcFlags->setItem(1, 1, new QTableWidgetItem(QObject::tr("AGGRESSIVO - Attacca i giocatori nella stanza")));
    ui->tableWidgetNpcFlags->setItem(2, 1, new QTableWidgetItem(QObject::tr("GUARDIA - Pattuglia la mappa")));
    ui->tableWidgetNpcFlags->setItem(3, 1, new QTableWidgetItem(QObject::tr("GUARDIA ZONA PICCOLA - Pattuglia una zona piccola in wild")));
    ui->tableWidgetNpcFlags->setItem(4, 1, new QTableWidgetItem(QObject::tr("GUARDIA ZONA GRANDE - Pattuglia una zona grande in wild")));
    ui->tableWidgetNpcFlags->setItem(5, 1, new QTableWidgetItem(QObject::tr("SEGUE TRACCE MAPPA - Segue senza uscire dalla mappa")));
    ui->tableWidgetNpcFlags->setItem(6, 1, new QTableWidgetItem(QObject::tr("ASSISTE - Assiste gli NPC dello stesso gruppo e mappa")));

    ui->tableWidgetNpcFlags->item(0, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::NpcCharacterFlagType::AUTOIA_COMBAT));
    ui->tableWidgetNpcFlags->item(1, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::NpcCharacterFlagType::AGGRESSIVE));
    ui->tableWidgetNpcFlags->item(2, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::NpcCharacterFlagType::PATROL));
    ui->tableWidgetNpcFlags->item(3, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_SMALL));
    ui->tableWidgetNpcFlags->item(4, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_LARGE));
    ui->tableWidgetNpcFlags->item(5, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::NpcCharacterFlagType::HUNT_MAP));
    ui->tableWidgetNpcFlags->item(6, 1)->setData(Qt::UserRole, static_cast<int>(QMUD::NpcCharacterFlagType::ASSIST));

    for (int i = 0; i < ui->tableWidgetNpcFlags->rowCount(); ++i)
    {
        ui->tableWidgetNpcFlags->setCellWidget(i, 0, new FormCenteredCheckBox());
        ui->tableWidgetNpcFlags->setRowHeight(i, 16);
    }

    ui->comboBoxNpcSex->addItem("Maschio", static_cast<int>(QMUD::SexType::MALE));
    ui->comboBoxNpcSex->addItem("Femmina", static_cast<int>(QMUD::SexType::FEMALE));

    for (int i = static_cast<int>(QMUD::RaceType::RACE_FIRST); i < static_cast<int>(QMUD::RaceType::RACE_TYPE_MAX); ++i)
        ui->comboBoxNpcRace->addItem(QMUD::Race::toReadableString(static_cast<QMUD::RaceType>(i)), i);

    ui->comboBoxNpcType->addItem("Normale",         static_cast<int>(QMUD::NpcType::NORMAL));
    ui->comboBoxNpcType->addItem("Elite",           static_cast<int>(QMUD::NpcType::ELITE));
    ui->comboBoxNpcType->addItem("Raro",            static_cast<int>(QMUD::NpcType::RARE));
    ui->comboBoxNpcType->addItem("Leggendario",     static_cast<int>(QMUD::NpcType::LEGENDARY));
    ui->comboBoxNpcType->addItem("Boss livello 1",  static_cast<int>(QMUD::NpcType::BOSS_A));
    ui->comboBoxNpcType->addItem("Boss livello 2",  static_cast<int>(QMUD::NpcType::BOSS_B));

    ui->comboBoxNpcClass->addItem("Guerriero",      static_cast<int>(QMUD::ClassType::WARRIOR));
    ui->comboBoxNpcClass->addItem("Mago",           static_cast<int>(QMUD::ClassType::MAGE));
    ui->comboBoxNpcClass->addItem("Ladro",          static_cast<int>(QMUD::ClassType::ROGUE));
    ui->comboBoxNpcClass->addItem("Chierico",       static_cast<int>(QMUD::ClassType::PRIEST));
    ui->comboBoxNpcClass->addItem("Druido",         static_cast<int>(QMUD::ClassType::DRUID));
    ui->comboBoxNpcClass->addItem("Barbaro",        static_cast<int>(QMUD::ClassType::BARBARIAN));
    ui->comboBoxNpcClass->addItem("Monaco",         static_cast<int>(QMUD::ClassType::MONK));

    ui->comboBoxItemQuality->addItem("Spazzatura",  static_cast<int>(QMUD::ItemQualityType::JUNK));
    ui->comboBoxItemQuality->addItem("Comune",      static_cast<int>(QMUD::ItemQualityType::COMMON));
    ui->comboBoxItemQuality->addItem("Non comune",  static_cast<int>(QMUD::ItemQualityType::UNCOMMON));
    ui->comboBoxItemQuality->addItem("Pregiato",    static_cast<int>(QMUD::ItemQualityType::VALUABLE));
    ui->comboBoxItemQuality->addItem("Raro",        static_cast<int>(QMUD::ItemQualityType::RARE));
    ui->comboBoxItemQuality->addItem("Epico",       static_cast<int>(QMUD::ItemQualityType::EPIC));
    ui->comboBoxItemQuality->addItem("Leggendario", static_cast<int>(QMUD::ItemQualityType::LEGENDARY));

    ui->comboBoxItemType->addItem("Base",       static_cast<int>(QMUD::ItemType::BASE));
    ui->comboBoxItemType->addItem("Arma",       static_cast<int>(QMUD::ItemType::WEAPON));
    ui->comboBoxItemType->addItem("Armatura",   static_cast<int>(QMUD::ItemType::ARMOR));
    ui->comboBoxItemType->addItem("Gioiello",   static_cast<int>(QMUD::ItemType::TRINKET));
    ui->comboBoxItemType->addItem("Cibo",       static_cast<int>(QMUD::ItemType::FOOD));
    ui->comboBoxItemType->addItem("Bevanda",    static_cast<int>(QMUD::ItemType::DRINK));
    ui->comboBoxItemType->addItem("Pozione",    static_cast<int>(QMUD::ItemType::POTION));
    ui->comboBoxItemType->addItem("Tonico",     static_cast<int>(QMUD::ItemType::TONIC));

    ui->comboBoxItemArmorType->addItem("Scudo",     static_cast<int>(QMUD::ArmorType::SHIELD));
    ui->comboBoxItemArmorType->addItem("Armatura",  static_cast<int>(QMUD::ArmorType::CHEST));
    ui->comboBoxItemArmorType->addItem("Guanti",    static_cast<int>(QMUD::ArmorType::GLOVES));
    ui->comboBoxItemArmorType->addItem("Spalline",  static_cast<int>(QMUD::ArmorType::SHOULDER));
    ui->comboBoxItemArmorType->addItem("Bracciali", static_cast<int>(QMUD::ArmorType::BRACERS));
    ui->comboBoxItemArmorType->addItem("Elmo",      static_cast<int>(QMUD::ArmorType::HELM));
    ui->comboBoxItemArmorType->addItem("Gambali",   static_cast<int>(QMUD::ArmorType::LEG));
    ui->comboBoxItemArmorType->addItem("Stivali",   static_cast<int>(QMUD::ArmorType::BOOTS));

    ui->comboBoxItemWeaponType->addItem("Clava",                static_cast<int>(QMUD::WeaponType::CLUB));
    ui->comboBoxItemWeaponType->addItem("Pugnale",              static_cast<int>(QMUD::WeaponType::DAGGER));
    ui->comboBoxItemWeaponType->addItem("Clava a due mani",     static_cast<int>(QMUD::WeaponType::TWO_HAND_CLUB));
    ui->comboBoxItemWeaponType->addItem("Ascia",                static_cast<int>(QMUD::WeaponType::AXE));
    ui->comboBoxItemWeaponType->addItem("Martello",             static_cast<int>(QMUD::WeaponType::LIGHT_HAMMER));
    ui->comboBoxItemWeaponType->addItem("Mazza",                static_cast<int>(QMUD::WeaponType::MACE));
    ui->comboBoxItemWeaponType->addItem("Bastone",              static_cast<int>(QMUD::WeaponType::QUARTERSTAFF));
    ui->comboBoxItemWeaponType->addItem("Falce",                static_cast<int>(QMUD::WeaponType::SICKLE));
    ui->comboBoxItemWeaponType->addItem("Ascia a due mani",     static_cast<int>(QMUD::WeaponType::TWO_HAND_AXE));
    ui->comboBoxItemWeaponType->addItem("Alabarda",             static_cast<int>(QMUD::WeaponType::HALBERD));
    ui->comboBoxItemWeaponType->addItem("Lancia",               static_cast<int>(QMUD::WeaponType::LANCE));
    ui->comboBoxItemWeaponType->addItem("Spada corta",          static_cast<int>(QMUD::WeaponType::SHORT_SWORD));
    ui->comboBoxItemWeaponType->addItem("Spada",                static_cast<int>(QMUD::WeaponType::SWORD));
    ui->comboBoxItemWeaponType->addItem("Spada lunga",          static_cast<int>(QMUD::WeaponType::LONG_SWORD));
    ui->comboBoxItemWeaponType->addItem("Morningstar",          static_cast<int>(QMUD::WeaponType::MORNINGSTAR));
    ui->comboBoxItemWeaponType->addItem("Picca",                static_cast<int>(QMUD::WeaponType::PIKE));
    ui->comboBoxItemWeaponType->addItem("Stocco",               static_cast<int>(QMUD::WeaponType::RAPIER));
    ui->comboBoxItemWeaponType->addItem("Scimitarra",           static_cast<int>(QMUD::WeaponType::SCIMITAR));
    ui->comboBoxItemWeaponType->addItem("Tridente",             static_cast<int>(QMUD::WeaponType::TRIDENT));
    ui->comboBoxItemWeaponType->addItem("Martello da guerra",   static_cast<int>(QMUD::WeaponType::WAR_HAMMER));
    ui->comboBoxItemWeaponType->addItem("Katana",               static_cast<int>(QMUD::WeaponType::KATANA));

    ui->comboBoxItemTrinketType->addItem("Anello",              static_cast<int>(QMUD::TrinketType::RING));
    ui->comboBoxItemTrinketType->addItem("Collana",             static_cast<int>(QMUD::TrinketType::NECKLACE));
    ui->comboBoxItemTrinketType->addItem("Bracciale",           static_cast<int>(QMUD::TrinketType::BRACELET));

    ui->comboBoxItemTemporaryStatus0->addItem("Nessuno",             static_cast<int>(QMUD::TemporaryStatusType::UNKNOWN));
    ui->comboBoxItemTemporaryStatus0->addItem("Santificazione",      static_cast<int>(QMUD::TemporaryStatusType::SANCTIFY));
    ui->comboBoxItemTemporaryStatus0->addItem("Scudo",               static_cast<int>(QMUD::TemporaryStatusType::SHIELD));
    ui->comboBoxItemTemporaryStatus0->addItem("Forza",               static_cast<int>(QMUD::TemporaryStatusType::STRENGHT));
    ui->comboBoxItemTemporaryStatus0->addItem("Volo",                static_cast<int>(QMUD::TemporaryStatusType::FLY));
    ui->comboBoxItemTemporaryStatus0->addItem("Invisibilita",        static_cast<int>(QMUD::TemporaryStatusType::INVISIBILITY));
    ui->comboBoxItemTemporaryStatus0->addItem("Vedi invisibilita",   static_cast<int>(QMUD::TemporaryStatusType::SEE_INVISIBILITY));

    ui->comboBoxItemTemporaryStatus1->addItem("Nessuno",             static_cast<int>(QMUD::TemporaryStatusType::UNKNOWN));
    ui->comboBoxItemTemporaryStatus1->addItem("Santificazione",      static_cast<int>(QMUD::TemporaryStatusType::SANCTIFY));
    ui->comboBoxItemTemporaryStatus1->addItem("Scudo",               static_cast<int>(QMUD::TemporaryStatusType::SHIELD));
    ui->comboBoxItemTemporaryStatus1->addItem("Forza",               static_cast<int>(QMUD::TemporaryStatusType::STRENGHT));
    ui->comboBoxItemTemporaryStatus1->addItem("Volo",                static_cast<int>(QMUD::TemporaryStatusType::FLY));
    ui->comboBoxItemTemporaryStatus1->addItem("Invisibilita",        static_cast<int>(QMUD::TemporaryStatusType::INVISIBILITY));
    ui->comboBoxItemTemporaryStatus1->addItem("Vedi invisibilita",   static_cast<int>(QMUD::TemporaryStatusType::SEE_INVISIBILITY));

    ui->comboBoxItemTemporaryStatus2->addItem("Nessuno",             static_cast<int>(QMUD::TemporaryStatusType::UNKNOWN));
    ui->comboBoxItemTemporaryStatus2->addItem("Santificazione",      static_cast<int>(QMUD::TemporaryStatusType::SANCTIFY));
    ui->comboBoxItemTemporaryStatus2->addItem("Scudo",               static_cast<int>(QMUD::TemporaryStatusType::SHIELD));
    ui->comboBoxItemTemporaryStatus2->addItem("Forza",               static_cast<int>(QMUD::TemporaryStatusType::STRENGHT));
    ui->comboBoxItemTemporaryStatus2->addItem("Volo",                static_cast<int>(QMUD::TemporaryStatusType::FLY));
    ui->comboBoxItemTemporaryStatus2->addItem("Invisibilita",        static_cast<int>(QMUD::TemporaryStatusType::INVISIBILITY));
    ui->comboBoxItemTemporaryStatus2->addItem("Vedi invisibilita",   static_cast<int>(QMUD::TemporaryStatusType::SEE_INVISIBILITY));

    ui->comboBoxItemClassType->addItem("Caster (Saggezza)",     static_cast<int>(QMUD::ItemClassType::CASTER_WIS));
    ui->comboBoxItemClassType->addItem("Caster (Intelligenza)", static_cast<int>(QMUD::ItemClassType::CASTER_INT));
    ui->comboBoxItemClassType->addItem("DPS (Destrezza)",       static_cast<int>(QMUD::ItemClassType::DPS_DEX));
    ui->comboBoxItemClassType->addItem("DPS (Forza)",           static_cast<int>(QMUD::ItemClassType::DPS_STR));
    ui->comboBoxItemClassType->addItem("Tank",                  static_cast<int>(QMUD::ItemClassType::TANK));

    int row = 0;
    for (int i = static_cast<int>(QMUD::ResourceType::RESOURCE_HERBALISM_FIRST);
         i < static_cast<int>(QMUD::ResourceType::RESOURCE_HERBALISM_MAX_VALUE);
         ++i, ++row)
    {
        ui->tableWidgetResources->setRowCount(ui->tableWidgetResources->rowCount()+1);
        ui->tableWidgetResources->setItem(row, 0, new QTableWidgetItem(QMUD::resourceToReadableString(static_cast<QMUD::ResourceType>(i))));
        ui->tableWidgetResources->item(row, 0)->setData(Qt::UserRole, i);
        ui->tableWidgetResources->setItem(row, 1, new QTableWidgetItem(QString::number(QMUD::resourceToLevel(static_cast<QMUD::ResourceType>(i)))));

        QComboBox *cbItemRarity = new QComboBox(this);

        cbItemRarity->addItem("-", static_cast<int>(QMUD::ResourceRarityType::NONE));
        cbItemRarity->addItem("NORMALE", static_cast<int>(QMUD::ResourceRarityType::NORMAL));
        cbItemRarity->addItem("NON COMUNE", static_cast<int>(QMUD::ResourceRarityType::UNCOMMON));
        cbItemRarity->addItem("RARA",  static_cast<int>(QMUD::ResourceRarityType::RARE));
        cbItemRarity->addItem("MOLTO RARA",  static_cast<int>(QMUD::ResourceRarityType::VERY_RARE));
        cbItemRarity->addItem("ESTREMAMENTE RARA",  static_cast<int>(QMUD::ResourceRarityType::EXTREMELY_RARE));

        cbItemRarity->setEditable(true);
        cbItemRarity->lineEdit()->setReadOnly(true);
        cbItemRarity->lineEdit()->setAlignment(Qt::AlignCenter);
        cbItemRarity->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        cbItemRarity->installEventFilter(this);

        ui->tableWidgetResources->setCellWidget(row, 2, cbItemRarity);
    }

    for (int i = static_cast<int>(QMUD::ResourceType::RESOURCE_MINING_FIRST);
         i < static_cast<int>(QMUD::ResourceType::RESOURCE_MINING_MAX_VALUE);
         ++i, ++row)
    {
        ui->tableWidgetResources->setRowCount(ui->tableWidgetResources->rowCount()+1);
        ui->tableWidgetResources->setItem(row, 0, new QTableWidgetItem(QMUD::resourceToReadableString(static_cast<QMUD::ResourceType>(i))));
        ui->tableWidgetResources->item(row, 0)->setData(Qt::UserRole, i);
        ui->tableWidgetResources->setItem(row, 1, new QTableWidgetItem(QString::number(QMUD::resourceToLevel(static_cast<QMUD::ResourceType>(i)))));

        QComboBox *cbItemRarity = new QComboBox(this);

        cbItemRarity->addItem("-", static_cast<int>(QMUD::ResourceRarityType::NONE));
        cbItemRarity->addItem("NORMALE", static_cast<int>(QMUD::ResourceRarityType::NORMAL));
        cbItemRarity->addItem("NON COMUNE", static_cast<int>(QMUD::ResourceRarityType::UNCOMMON));
        cbItemRarity->addItem("RARA",  static_cast<int>(QMUD::ResourceRarityType::RARE));
        cbItemRarity->addItem("MOLTO RARA",  static_cast<int>(QMUD::ResourceRarityType::VERY_RARE));
        cbItemRarity->addItem("ESTREMAMENTE RARA",  static_cast<int>(QMUD::ResourceRarityType::EXTREMELY_RARE));

        cbItemRarity->setEditable(true);
        cbItemRarity->lineEdit()->setReadOnly(true);
        cbItemRarity->lineEdit()->setAlignment(Qt::AlignCenter);
        cbItemRarity->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        cbItemRarity->installEventFilter(this);

        ui->tableWidgetResources->setCellWidget(row, 2, cbItemRarity);
    }

    ui->tableWidgetResources->resizeColumnToContents(0);
    ui->tableWidgetResources->resizeColumnToContents(1);

    // Inizializzo l'editor delle mappe tiles
    m_ptrMapEditor = new FormTilesMapEditor();
    m_ptrMapEditor->setWindowModality(Qt::WindowModal);

    connect(m_ptrMapEditor, SIGNAL(editComplete()), this, SLOT(onEditorMapTilesEditCompleted()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (ui->treeWidgetMaps->topLevelItemCount() > 0)
    {
        auto ret = QMessageBox::question(this, tr("Salvataggio"), tr("Vuoi salvare le modifiche correnti?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);

        if (ret == QMessageBox::Yes)
        {
            if (saveAll())
            {
                event->accept();
                return;
            }
            else
            {
                if (QMessageBox::question(this, tr("Salvataggio fallito"), tr("Vuoi chiudere comunque il software"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
                    == QMessageBox::Yes)
                {
                    event->accept();
                    return;
                }
            }
        }
        else if (ret == QMessageBox::No)
        {

            event->accept();
            return;
        }

        event->ignore();
        return;
    }

    event->accept();
    return;
}

void MainWindow::onEditorMapTilesEditCompleted()
{
    EditorMap::MapTilesData map;
    if (!m_ptrMapEditor->getMap(map))
        return;

    for (int i=0; i< ui->tableWidgetTilesMaps->rowCount(); ++i)
    {
        if (ui->tableWidgetTilesMaps->item(i, 1)->data(Qt::UserRole).value<EditorMap::MapTilesData>().mapIndex() == map.mapIndex())
        {
            ui->tableWidgetTilesMaps->item(i, 1)->setData(Qt::UserRole, QVariant::fromValue(map));
            ui->tableWidgetTilesMaps->item(i, 1)->setText(QString::number(map.mapIndex()));
            ui->tableWidgetTilesMaps->item(i, 2)->setText(map.mapName());
        }
    }
}

void MainWindow::addMapToTree(EditorMap* map)
{
    ui->stackedWidget->setCurrentIndex(0);

    TreeWidgetAction *actCloseMap = new TreeWidgetAction();

    QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetMaps);

    QAction *actionCloseMap = new QAction();
    actionCloseMap->setIcon(QIcon(":/icons/icons/remove.png"));

    QObject::connect(actionCloseMap, &QAction::triggered, [this, map, item](){
        auto ret = QMessageBox::question(this, tr("Chiusura"), tr("Vuoi salvare la mappa %1 prima di chiuderla?").arg(map->name()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);

        if (ret == QMessageBox::Yes)
        {
            if (item == m_widgetMapOnInterface)
                toMapOnInterface();

            if (map->filePath().simplified().isEmpty())
            {
                QString fileName = QFileDialog::getSaveFileName(this, tr("Seleziona dove salvare la mappa %1").arg(map->name()),
                                                                "",
                                                                tr("Json file (*.json)"));

                if (!fileName.simplified().isEmpty())
                    map->setFilePath(fileName);
                else
                    return;
            }

            if (!map->write(m_ptrMapEditor))
            {
                QMessageBox::critical(this, "Errore", tr("Errore durante il salvataggio del file %1").arg(map->filePath()));
                return;
            }
        }
        else if (ret == QMessageBox::Cancel)
            return;

        delete item;

        if (ui->treeWidgetMaps->topLevelItemCount() <= 0)
            ui->stackedWidget->setCurrentIndex(1);
    });

    actCloseMap->addAction(actionCloseMap);
    actCloseMap->setText(QString("%1 (Id: %2)").arg(map->name()).arg(map->id()));

    item->setData(0, Qt::UserRole, QVariant::fromValue<EditorMap*>(map));
    ui->treeWidgetMaps->addTopLevelItem(item);
    ui->treeWidgetMaps->setItemWidget(item, 0, actCloseMap);

    auto childNpcs = new QTreeWidgetItem(item);
    auto childItems = new QTreeWidgetItem(item);

    item->addChild(childNpcs);
    item->addChild(childItems);

    TreeWidgetAction *actNpcs = new TreeWidgetAction();
    TreeWidgetAction *actItems = new TreeWidgetAction();

    QAction *actionAddNpc = new QAction();
    actionAddNpc->setIcon(QIcon(":/icons/icons/add.png"));

    QObject::connect(actionAddNpc, &QAction::triggered, [this, map, item](){
        auto npc = new EditorNpc(map);
        npc->setId(map->unusedNpcId());
        map->addNpc(npc);
        addNpcToTree(npc, item);
    });

    actNpcs->addAction(actionAddNpc);

    QAction *actionAddItem = new QAction();
    actionAddItem->setIcon(QIcon(":/icons/icons/add.png"));

    QObject::connect(actionAddItem, &QAction::triggered, [this, map, item](){
        auto newItem = new EditorItem(map);
        newItem->setId(map->unusedNpcId());
        map->addItem(newItem);
        addItemToTree(newItem, item);
    });

    actItems->addAction(actionAddItem);

    actNpcs->setText("Npc");
    actItems->setText("Oggetti");

    ui->treeWidgetMaps->setItemWidget(childNpcs, 0, actNpcs);
    ui->treeWidgetMaps->setItemWidget(childItems, 0, actItems);

    for (auto npc : map->npcs())
        addNpcToTree(npc, item);

    for (auto it : map->items())
        addItemToTree(it, item);

    ui->pushButtonSave->setEnabled(true);

    if (ui->treeWidgetMaps->topLevelItemCount() == 1)
    {
        ui->treeWidgetMaps->topLevelItem(0)->setExpanded(true);
        ui->treeWidgetMaps->topLevelItem(0)->child(0)->setExpanded(true);
        ui->treeWidgetMaps->topLevelItem(0)->child(1)->setExpanded(true);
    }
}

void MainWindow::addNpcToTree(EditorNpc *npc, QTreeWidgetItem *itemMap)
{
    QTreeWidgetItem* itemNpcs = itemMap->child(0);

    TreeWidgetAction *wdgNpc = new TreeWidgetAction();

    wdgNpc->setText(QString("%1 (Id: %2)").arg(npc->name()).arg(npc->id()));

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(itemNpcs);
    ui->treeWidgetMaps->setItemWidget(treeItem, 0, wdgNpc);

    treeItem->setData(0, Qt::UserRole, QVariant::fromValue<EditorNpc*>(npc));
    itemNpcs->addChild(treeItem);

    QAction *action = new QAction();
    action->setIcon(QIcon(":/icons/icons/remove.png"));

    QObject::connect(action, &QAction::triggered, [this, npc, itemMap, treeItem]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Attenzione", "Vuoi rimuovere l'NPC " + npc->name() + "?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
            return;

        auto npcToRemove = npc;
        auto npcToRemoveMap = itemMap->data(0, Qt::UserRole).value<EditorMap*>();

        if (npc == m_npcOnInterface && npcToRemoveMap == m_npcOnInterfaceMap)
            ui->treeWidgetMaps->clearSelection();

        // Rimuovo l'oggetto dalla mappa
        auto npcs = npcToRemoveMap->npcs();
        npcs.removeOne(npcToRemove);
        npcToRemoveMap->setNpcs(npcs);

        // Rimuovo l'itemTree dall'albero
        delete treeItem;

        // Deleto l'npc
        delete npcToRemove;
    });

    wdgNpc->addAction(action, false);
}

void MainWindow::addItemToTree(EditorItem *item, QTreeWidgetItem *itemMap)
{
    QTreeWidgetItem* itemItems = itemMap->child(1);

    TreeWidgetAction *wdgItem = new TreeWidgetAction();

    wdgItem->setText(QString("%1 (Id: %2)").arg(item->name()).arg(item->id()));

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(itemItems);
    ui->treeWidgetMaps->setItemWidget(treeItem, 0, wdgItem);

    treeItem->setData(0, Qt::UserRole, QVariant::fromValue<EditorItem*>(item));
    itemItems->addChild(treeItem);

    QAction *action = new QAction();
    action->setIcon(QIcon(":/icons/icons/remove.png"));

    QObject::connect(action, &QAction::triggered, [this, item, itemMap, treeItem]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Attenzione", "Vuoi rimuovere l'oggetto " + item->name() + "?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
            return;

        auto itemToRemove = item;
        auto itemToRemoveMap = itemMap->data(0, Qt::UserRole).value<EditorMap*>();

        if (item == m_itemOnInterface && itemToRemoveMap == m_itemOnInterfaceMap)
            ui->treeWidgetMaps->clearSelection();

        // Rimuovo i riferimenti nel loot degli NPC
        auto npcs = itemToRemoveMap->npcs();

        bool modNpcs = false;
        for (auto& npc : npcs)
        {
            bool modLoot = false;
            auto loot = npc->loot();
            for (auto it = loot.begin(); it != loot.end(); )
                if (it->id == itemToRemove->id())
                {
                    modLoot = true;
                    it = loot.erase(it);
                }
                else
                    ++it;

            if (modLoot)
            {
                npc->setLoot(loot);
                modNpcs = true;
            }
        }

        if (modNpcs)
            itemToRemoveMap->setNpcs(npcs);

        // Rimuovo l'oggetto dalla mappa
        auto items = itemToRemoveMap->items();
        items.removeOne(itemToRemove);
        itemToRemoveMap->setItems(items);

        // Rimuovo l'itemTree dall'albero
        delete treeItem;

        // Deleto l'item
        delete itemToRemove;
    });

    wdgItem->addAction(action, false);
}

void MainWindow::on_pushButtonNew_clicked()
{
    addMapToTree(new EditorMap());
}

void MainWindow::on_treeWidgetMaps_itemSelectionChanged()
{
    qDebug() << "sel";
    toMapOnInterface();
    m_mapOnInterface = Q_NULLPTR;
    m_widgetMapOnInterface = Q_NULLPTR;
    fromMapOnInterface();

    toNpcOnInterface();
    m_npcOnInterface = Q_NULLPTR;
    m_npcOnInterfaceMap = Q_NULLPTR;
    m_widgetNpcOnInterface = Q_NULLPTR;
    fromNpcOnInterface();

    toItemOnInterface();
    m_itemOnInterface = Q_NULLPTR;
    m_itemOnInterfaceMap = Q_NULLPTR;
    m_widgetItemOnInterface = Q_NULLPTR;
    fromItemOnInterface();

    if (!ui->treeWidgetMaps->selectedItems().isEmpty() && ui->treeWidgetMaps->selectedItems()[0]->data(0, Qt::UserRole).isValid())
    {
        // Selezionata una mappa
        if (ui->treeWidgetMaps->selectedItems()[0]->data(0, Qt::UserRole).canConvert<EditorMap*>())
        {
            auto selectedMap = ui->treeWidgetMaps->selectedItems()[0]->data(0, Qt::UserRole).value<EditorMap*>();

            toMapOnInterface();
            m_mapOnInterface = selectedMap;
            m_widgetMapOnInterface = ui->treeWidgetMaps->selectedItems()[0];
            fromMapOnInterface();
        }

        // Selezionato un npc
        if (ui->treeWidgetMaps->selectedItems()[0]->data(0, Qt::UserRole).canConvert<EditorNpc*>())
        {
            auto selectedNpc = ui->treeWidgetMaps->selectedItems()[0]->data(0, Qt::UserRole).value<EditorNpc*>();

            toNpcOnInterface();
            m_npcOnInterface = selectedNpc;
            m_npcOnInterfaceMap = ui->treeWidgetMaps->selectedItems()[0]->parent()->parent()->data(0, Qt::UserRole).value<EditorMap*>();
            m_widgetNpcOnInterface = ui->treeWidgetMaps->selectedItems()[0];
            fromNpcOnInterface();
        }

        // Selezionato un item
        if (ui->treeWidgetMaps->selectedItems()[0]->data(0, Qt::UserRole).canConvert<EditorItem*>())
        {
            auto selectedItem = ui->treeWidgetMaps->selectedItems()[0]->data(0, Qt::UserRole).value<EditorItem*>();

            toItemOnInterface();
            m_itemOnInterface = selectedItem;
            m_itemOnInterfaceMap = ui->treeWidgetMaps->selectedItems()[0]->parent()->parent()->data(0, Qt::UserRole).value<EditorMap*>();
            m_widgetItemOnInterface = ui->treeWidgetMaps->selectedItems()[0];
            fromItemOnInterface();
        }
    }
}

void MainWindow::fromMapOnInterface()
{
    if (!m_mapOnInterface)
    {
        ui->stackedWidgetMain->setCurrentWidget(ui->pageEmpty);
        return;
    }

    ui->stackedWidgetMain->setCurrentWidget(ui->pageGeneral);
    ui->spinBoxMapID->setValue(m_mapOnInterface->id());
    ui->lineEditMapName->setText(m_mapOnInterface->name());

    for (int row=0; row<ui->tableWidgetResources->rowCount(); ++row)
    {
        QComboBox* cb = static_cast<QComboBox*>(ui->tableWidgetResources->cellWidget(row, 2));

        for (int i = 0; i < cb->count(); ++i)
        {
            if (static_cast<QMUD::ResourceRarityType>(cb->itemData(i).toInt()) == QMUD::ResourceRarityType::NONE)
            {
                cb->setCurrentIndex(i);
                break;
            }
        }
    }

    auto resources = m_mapOnInterface->resources();

    for (auto info : resources)
    {
        for (int row=0; row<ui->tableWidgetResources->rowCount(); ++row)
        {
            QMUD::ResourceType resource = static_cast<QMUD::ResourceType>(ui->tableWidgetResources->item(row, 0)->data(Qt::UserRole).toInt());

            if (resource == info.resource)
            {
                QComboBox* cb = static_cast<QComboBox*>(ui->tableWidgetResources->cellWidget(row, 2));

                for (int i = 0; i < cb->count(); ++i)
                {
                    if (static_cast<QMUD::ResourceRarityType>(cb->itemData(i).toInt()) == info.rarity)
                    {
                        cb->setCurrentIndex(i);
                        break;
                    }
                }

                break;
            }
        }
    }

    auto quests = m_mapOnInterface->quests();
    ui->tableWidgetQuests->setRowCount(0);

    for (auto quest : quests)
        addQuest(quest);

    auto maps = m_mapOnInterface->mapTiles();
    ui->tableWidgetTilesMaps->setRowCount(0);

    for (auto map : maps)
        addTilesMap(map);
}

void MainWindow::toMapOnInterface()
{
    if (!m_mapOnInterface)
        return;

    m_mapOnInterface->setId(ui->spinBoxMapID->value());
    m_mapOnInterface->setName(ui->lineEditMapName->text());

    // Risorse
    QVector<EditorMap::ResourceInfo> resources;

    for (int row=0; row<ui->tableWidgetResources->rowCount(); ++row)
    {
        QMUD::ResourceType resource = static_cast<QMUD::ResourceType>(ui->tableWidgetResources->item(row, 0)->data(Qt::UserRole).toInt());
        QMUD::ResourceRarityType rarity = static_cast<QMUD::ResourceRarityType>(static_cast<QComboBox*>(ui->tableWidgetResources->cellWidget(row, 2))->currentData().toInt());

        if (rarity != QMUD::ResourceRarityType::NONE)
        {
            EditorMap::ResourceInfo info;
            info.resource = resource;
            info.rarity = rarity;
            resources.push_back(info);
        }
    }

    m_mapOnInterface->setResources(resources);

    // Quest
    QVector<EditorMap::QuestData> quests;

    for (int row=0; row<ui->tableWidgetQuests->rowCount(); ++row)
    {
        EditorMap::QuestData questData = ui->tableWidgetQuests->item(row, 1)->data(Qt::UserRole).value<EditorMap::QuestData>();

        quests.push_back(questData);
    }

    m_mapOnInterface->setQuests(quests);

    // Mappe
    QVector<EditorMap::MapTilesData> maps;

    for (int row=0; row<ui->tableWidgetTilesMaps->rowCount(); ++row)
    {
        EditorMap::MapTilesData mapData = ui->tableWidgetTilesMaps->item(row, 1)->data(Qt::UserRole).value<EditorMap::MapTilesData>();

        maps.push_back(mapData);
    }

    m_mapOnInterface->setMapTiles(maps);
}

void MainWindow::fromNpcOnInterface()
{
    if (!m_npcOnInterface)
    {
        ui->stackedWidgetMain->setCurrentWidget(ui->pageEmpty);
        return;
    }

    ui->stackedWidgetMain->setCurrentWidget(ui->pageNpc);
    ui->spinBoxNpcId->setValue(m_npcOnInterface->id());
    ui->lineEditNpcName->setText(m_npcOnInterface->name());

    for (int i = 0; i < ui->comboBoxNpcSex->count(); ++i)
        if (ui->comboBoxNpcSex->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_npcOnInterface->sex()))
        {
            ui->comboBoxNpcSex->setCurrentIndex(i);
            break;
        }

    for (int i = 0; i < ui->comboBoxNpcRace->count(); ++i)
        if (ui->comboBoxNpcRace->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_npcOnInterface->race()))
        {
            ui->comboBoxNpcRace->setCurrentIndex(i);
            break;
        }

    for (int i = 0; i < ui->comboBoxNpcClass->count(); ++i)
        if (ui->comboBoxNpcClass->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_npcOnInterface->classType()))
        {
            ui->comboBoxNpcClass->setCurrentIndex(i);
            break;
        }

    for (int i = 0; i < ui->comboBoxNpcType->count(); ++i)
        if (ui->comboBoxNpcType->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_npcOnInterface->type()))
        {
            ui->comboBoxNpcType->setCurrentIndex(i);
            break;
        }

    ui->spinBoxNpcBonusAttacks->setValue(m_npcOnInterface->bonusAttacks());

    ui->plainTextEditNpcDescription->clear();
    ui->plainTextEditNpcDescription->appendPlainText(m_npcOnInterface->description());

    ui->spinBoxNpcLevel->setValue(m_npcOnInterface->level());


    for (int i=0; i<ui->tableWidgetNpcFlags->rowCount(); ++i)
        (static_cast<FormCenteredCheckBox*>(ui->tableWidgetNpcFlags->cellWidget(i, 0)))->checkBox()->setChecked(false);

    auto flags = m_npcOnInterface->flags();

    for (auto flag : flags)
    {
        for (int i=0; i<ui->tableWidgetNpcFlags->rowCount(); ++i)
        {
            if (ui->tableWidgetNpcFlags->item(i, 1)->data(Qt::UserRole).toInt() == static_cast<int>(flag))
            {
                (static_cast<FormCenteredCheckBox*>(ui->tableWidgetNpcFlags->cellWidget(i, 0)))->checkBox()->setChecked(true);
                break;
            }
        }
    }

    ui->spinBoxNpcAssistGroup->setValue(m_npcOnInterface->assistGroup());

    ui->lineEditNpcEventCustom->setText(QString("%1%2").arg(m_npcOnInterface->eventCustom().left(30)).arg(m_npcOnInterface->eventCustom().size() > 30 ? "..." : ""));
    ui->lineEditNpcEventEnter->setText(QString("%1%2").arg(m_npcOnInterface->eventEnter().left(30)).arg(m_npcOnInterface->eventEnter().size() > 30 ? "..." : ""));
    ui->lineEditNpcEventAbility->setText(QString("%1%2").arg(m_npcOnInterface->eventAbility().left(30)).arg(m_npcOnInterface->eventAbility().size() > 30 ? "..." : ""));

    ui->pushButtonNpcLootAdd->setEnabled(!m_npcOnInterfaceMap->items().isEmpty());

    ui->tableWidgetNpcLoot->setRowCount(0);

    auto loot = m_npcOnInterface->loot();
    for (auto item : loot)
        addNpcLootItem(item.id, item.rarity);

    ui->groupBoxNpcShop->setChecked(m_npcOnInterface->shopEnabled());
    ui->pushButtonNpcShopAddItem->setEnabled(!m_npcOnInterfaceMap->items().isEmpty());

    if (m_npcOnInterface->shopEnabled())
    {
        ui->doubleSpinBoxNpcSaleMult->setValue(m_npcOnInterface->shopSaleMultiplier());
        ui->doubleSpinBoxNpcPurchaseMult->setValue(m_npcOnInterface->shopPurchaseMultiplier());
        ui->checkBoxNpcPurchaseBase->setChecked(m_npcOnInterface->shopPurchaseBase());
        ui->checkBoxNpcPurchaseArmor->setChecked(m_npcOnInterface->shopPurchaseArmor());
        ui->checkBoxNpcPurchaseWeapon->setChecked(m_npcOnInterface->shopPurchaseWeapon());

        ui->tableWidgetNpcShopItems->setRowCount(0);

        auto shopList = m_npcOnInterface->shopList();
        for (auto item : shopList)
            addNpcShopItem(item);
    }
    else
    {
        ui->doubleSpinBoxNpcSaleMult->setValue(1.0);
        ui->doubleSpinBoxNpcPurchaseMult->setValue(1.0);
        ui->checkBoxNpcPurchaseBase->setChecked(false);
        ui->checkBoxNpcPurchaseArmor->setChecked(false);
        ui->checkBoxNpcPurchaseWeapon->setChecked(false);
        ui->tableWidgetNpcShopItems->setRowCount(0);
    }
}

void MainWindow::toNpcOnInterface()
{
    if (!m_npcOnInterface)
    {
        ui->stackedWidgetMain->setCurrentWidget(ui->pageGeneral);
        return;
    }

    m_npcOnInterface->setId(ui->spinBoxNpcId->value());
    m_npcOnInterface->setName(ui->lineEditNpcName->text());
    m_npcOnInterface->setDescription(ui->plainTextEditNpcDescription->toPlainText());
    m_npcOnInterface->setSex(static_cast<QMUD::SexType>(ui->comboBoxNpcSex->currentData().toInt()));
    m_npcOnInterface->setRace(static_cast<QMUD::RaceType>(ui->comboBoxNpcRace->currentData().toInt()));
    m_npcOnInterface->setClassType(static_cast<QMUD::ClassType>(ui->comboBoxNpcClass->currentData().toInt()));
    m_npcOnInterface->setType(static_cast<QMUD::NpcType>(ui->comboBoxNpcType->currentData().toInt()));
    m_npcOnInterface->setBonusAttacks(ui->spinBoxNpcBonusAttacks->value());
    m_npcOnInterface->setLevel(ui->spinBoxNpcLevel->value());

    QVector<QMUD::NpcCharacterFlagType> flags;

    for (int i=0; i<ui->tableWidgetNpcFlags->rowCount(); ++i)
        if ((static_cast<FormCenteredCheckBox*>(ui->tableWidgetNpcFlags->cellWidget(i, 0)))->checkBox()->isChecked())
            flags.push_back(static_cast<QMUD::NpcCharacterFlagType>(ui->tableWidgetNpcFlags->item(i, 1)->data(Qt::UserRole).toInt()));

    m_npcOnInterface->setFlags(flags);
    m_npcOnInterface->setAssistGroup(ui->spinBoxNpcAssistGroup->value());

    QVector<EditorNpc::LootItemInfo> loot;

    for (auto i = 0; i<ui->tableWidgetNpcLoot->rowCount(); ++i)
    {
        EditorNpc::LootItemInfo item;
        item.id = qobject_cast<QComboBox*>(ui->tableWidgetNpcLoot->cellWidget(i, 1))->currentData().toInt();
        item.rarity = static_cast<QMUD::ItemRarityType>(qobject_cast<QComboBox*>(ui->tableWidgetNpcLoot->cellWidget(i, 2))->currentData().toInt());
        loot.push_back(item);
    }

    m_npcOnInterface->setLoot(loot);

    if (ui->groupBoxNpcShop->isChecked())
    {
        m_npcOnInterface->setShopEnabled(true);
        m_npcOnInterface->setShopSaleMultiplier(ui->doubleSpinBoxNpcSaleMult->value());
        m_npcOnInterface->setShopPurchaseMultiplier(ui->doubleSpinBoxNpcPurchaseMult->value());
        m_npcOnInterface->setShopPurchaseBase(ui->checkBoxNpcPurchaseBase->isChecked());
        m_npcOnInterface->setShopPurchaseArmor(ui->checkBoxNpcPurchaseArmor->isChecked());
        m_npcOnInterface->setShopPurchaseWeapon(ui->checkBoxNpcPurchaseWeapon->isChecked());

        QVector<int> shopItems;

        for (auto i = 0; i<ui->tableWidgetNpcShopItems->rowCount(); ++i)
        {
            shopItems.push_back(qobject_cast<QComboBox*>(ui->tableWidgetNpcShopItems->cellWidget(i, 1))->currentData().toInt());
        }

        m_npcOnInterface->setShopList(shopItems);
    }
    else
    {
        m_npcOnInterface->setShopEnabled(false);
    }
}

void MainWindow::fromItemOnInterface()
{
    if (!m_itemOnInterface)
    {
        ui->stackedWidgetMain->setCurrentWidget(ui->pageEmpty);
        return;
    }

    ui->stackedWidgetMain->setCurrentWidget(ui->pageItem);
    ui->spinBoxItemId->setValue(m_itemOnInterface->id());
    ui->spinBoxItemLevel->setValue(m_itemOnInterface->level());
    ui->lineEditItemName->setText(m_itemOnInterface->name());
    ui->plainTextEditItemDescription->clear();
    ui->plainTextEditItemDescription->appendPlainText(m_itemOnInterface->description());

    for (int i = 0; i < ui->comboBoxItemType->count(); ++i)
        if (ui->comboBoxItemType->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_itemOnInterface->type()))
        {
            ui->comboBoxItemType->setCurrentIndex(i);
            break;
        }

    for (int i = 0; i < ui->comboBoxItemQuality->count(); ++i)
        if (ui->comboBoxItemQuality->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_itemOnInterface->quality()))
        {
            ui->comboBoxItemQuality->setCurrentIndex(i);
            break;
        }

    for (int i = 0; i < ui->comboBoxItemArmorType->count(); ++i)
        if (ui->comboBoxItemArmorType->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_itemOnInterface->armorType()))
        {
            ui->comboBoxItemArmorType->setCurrentIndex(i);
            break;
        }

    for (int i = 0; i < ui->comboBoxItemWeaponType->count(); ++i)
        if (ui->comboBoxItemWeaponType->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_itemOnInterface->weaponType()))
        {
            ui->comboBoxItemWeaponType->setCurrentIndex(i);
            break;
        }

    for (int i = 0; i < ui->comboBoxItemTrinketType->count(); ++i)
        if (ui->comboBoxItemTrinketType->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_itemOnInterface->trinketType()))
        {
            ui->comboBoxItemTrinketType->setCurrentIndex(i);
            break;
        }

    ui->spinBoxItemDrinkMPGainBonus->setValue(m_itemOnInterface->drinkMPGainBonus());
    ui->spinBoxItemFoodHPGainBonus->setValue(m_itemOnInterface->foodHPGainBonus());
    ui->spinBoxItemFoodAPGainBonus->setValue(m_itemOnInterface->foodAPGainBonus());
    ui->spinBoxItemPotionHPGain->setValue(m_itemOnInterface->potionHP());
    ui->spinBoxItemPotionMPGain->setValue(m_itemOnInterface->potionMP());
    ui->spinBoxItemPotionAPGain->setValue(m_itemOnInterface->potionAP());
    ui->spinBoxItemPotionDurationSeconds->setValue(m_itemOnInterface->potionDurationSeconds());
    ui->spinBoxItemTonicDurationSeconds->setValue(m_itemOnInterface->tonicDurationSeconds());

    ui->comboBoxItemTemporaryStatus0->setCurrentIndex(0);
    ui->comboBoxItemTemporaryStatus1->setCurrentIndex(0);
    ui->comboBoxItemTemporaryStatus2->setCurrentIndex(0);

    if (m_itemOnInterface->baseStatus().temporaryStatus().size() > 0)
        for (int i = 0; i < ui->comboBoxItemTemporaryStatus0->count(); ++i)
            if (ui->comboBoxItemTemporaryStatus0->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_itemOnInterface->baseStatus().temporaryStatus()[0]))
            {
                ui->comboBoxItemTemporaryStatus0->setCurrentIndex(i);
                break;
            }

    if (m_itemOnInterface->baseStatus().temporaryStatus().size() > 1)
        for (int i = 0; i < ui->comboBoxItemTemporaryStatus1->count(); ++i)
            if (ui->comboBoxItemTemporaryStatus1->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_itemOnInterface->baseStatus().temporaryStatus()[1]))
            {
                ui->comboBoxItemTemporaryStatus1->setCurrentIndex(i);
                break;
            }

    if (m_itemOnInterface->baseStatus().temporaryStatus().size() > 2)
        for (int i = 0; i < ui->comboBoxItemTemporaryStatus2->count(); ++i)
            if (ui->comboBoxItemTemporaryStatus2->itemData(i, Qt::UserRole).toInt() == static_cast<int>(m_itemOnInterface->baseStatus().temporaryStatus()[2]))
            {
                ui->comboBoxItemTemporaryStatus2->setCurrentIndex(i);
                break;
            }

    ui->spinBoxItemCon->setValue(m_itemOnInterface->baseBonus().constitution());
    ui->spinBoxItemDex->setValue(m_itemOnInterface->baseBonus().dexterity());
    ui->spinBoxItemInt->setValue(m_itemOnInterface->baseBonus().intelligence());
    ui->spinBoxItemWis->setValue(m_itemOnInterface->baseBonus().wisdom());
    ui->spinBoxItemStr->setValue(m_itemOnInterface->baseBonus().strength());
    ui->spinBoxItemHit->setValue(m_itemOnInterface->baseBonus().hit());
    ui->spinBoxItemDamage->setValue(m_itemOnInterface->baseBonus().damage());
    ui->spinBoxItemHP->setValue(m_itemOnInterface->baseBonus().hitPoints());
    ui->spinBoxItemMP->setValue(m_itemOnInterface->baseBonus().manaPoints());
    ui->spinBoxItemAP->setValue(m_itemOnInterface->baseBonus().actionPoints());
}

void MainWindow::toItemOnInterface()
{
    if (!m_itemOnInterface)
    {
        ui->stackedWidgetMain->setCurrentWidget(ui->pageGeneral);
        return;
    }

    m_itemOnInterface->setId(ui->spinBoxItemId->value());
    m_itemOnInterface->setName(ui->lineEditItemName->text());
    m_itemOnInterface->setLevel(ui->spinBoxItemLevel->value());
    m_itemOnInterface->setDescription(ui->plainTextEditItemDescription->toPlainText());
    m_itemOnInterface->setType(static_cast<QMUD::ItemType>(ui->comboBoxItemType->currentData().toInt()));
    m_itemOnInterface->setQuality(static_cast<QMUD::ItemQualityType>(ui->comboBoxItemQuality->currentData().toInt()));
    m_itemOnInterface->setArmorType(static_cast<QMUD::ArmorType>(ui->comboBoxItemArmorType->currentData().toInt()));
    m_itemOnInterface->setWeaponType(static_cast<QMUD::WeaponType>(ui->comboBoxItemWeaponType->currentData().toInt()));
    m_itemOnInterface->setTrinketType(static_cast<QMUD::TrinketType>(ui->comboBoxItemTrinketType->currentData().toInt()));
    m_itemOnInterface->setDrinkMPGainBonus(ui->spinBoxItemDrinkMPGainBonus->value());
    m_itemOnInterface->setFoodHPGainBonus(ui->spinBoxItemFoodHPGainBonus->value());
    m_itemOnInterface->setFoodAPGainBonus(ui->spinBoxItemFoodAPGainBonus->value());
    m_itemOnInterface->setPotionHP(ui->spinBoxItemPotionHPGain->value());
    m_itemOnInterface->setPotionMP(ui->spinBoxItemPotionMPGain->value());
    m_itemOnInterface->setPotionAP(ui->spinBoxItemPotionAPGain->value());
    m_itemOnInterface->setPotionDurationSeconds(ui->spinBoxItemPotionDurationSeconds->value());
    m_itemOnInterface->setTonicDurationSeconds(ui->spinBoxItemTonicDurationSeconds->value());

    QVector<QMUD::TemporaryStatusType> baseStatus;
    if (static_cast<QMUD::TemporaryStatusType>(ui->comboBoxItemTemporaryStatus0->currentData().toInt()) != QMUD::TemporaryStatusType::UNKNOWN)
        baseStatus.push_back(static_cast<QMUD::TemporaryStatusType>(ui->comboBoxItemTemporaryStatus0->currentData().toInt()));
    if (static_cast<QMUD::TemporaryStatusType>(ui->comboBoxItemTemporaryStatus1->currentData().toInt()) != QMUD::TemporaryStatusType::UNKNOWN)
        baseStatus.push_back(static_cast<QMUD::TemporaryStatusType>(ui->comboBoxItemTemporaryStatus1->currentData().toInt()));
    if (static_cast<QMUD::TemporaryStatusType>(ui->comboBoxItemTemporaryStatus2->currentData().toInt()) != QMUD::TemporaryStatusType::UNKNOWN)
        baseStatus.push_back(static_cast<QMUD::TemporaryStatusType>(ui->comboBoxItemTemporaryStatus2->currentData().toInt()));

    QMUD::ItemBaseStatus status;
    status.setTemporaryStatus(baseStatus);
    m_itemOnInterface->setBaseStatus(status);

    QMUD::ItemBaseBonus baseBonus;
    baseBonus.setConstitution(ui->spinBoxItemCon->value());
    baseBonus.setDexterity(ui->spinBoxItemDex->value());
    baseBonus.setIntelligence(ui->spinBoxItemInt->value());
    baseBonus.setWisdom(ui->spinBoxItemWis->value());
    baseBonus.setStrength(ui->spinBoxItemStr->value());
    baseBonus.setHit(ui->spinBoxItemHit->value());
    baseBonus.setDamage(ui->spinBoxItemDamage->value());
    baseBonus.setHitPoints(ui->spinBoxItemHP->value());
    baseBonus.setManaPoints(ui->spinBoxItemMP->value());
    baseBonus.setActionPoints(ui->spinBoxItemAP->value());
    m_itemOnInterface->setBaseBonus(baseBonus);
}

EditorMap *MainWindow::findMap(int mapId) const
{
    for (int i=0; i<ui->treeWidgetMaps->topLevelItemCount(); ++i)
    {
        auto item = ui->treeWidgetMaps->topLevelItem(i);
        EditorMap* map = item->data(0, Qt::UserRole).value<EditorMap*>();

        if (map->id() == mapId)
        {
            return map;
        }
    }

    return Q_NULLPTR;
}

void MainWindow::addNpcLootItem(int itemId, QMUD::ItemRarityType rarity)
{
    ui->tableWidgetNpcLoot->setRowCount(ui->tableWidgetNpcLoot->rowCount() + 1);
    int row = ui->tableWidgetNpcLoot->rowCount() - 1;

    auto items = m_npcOnInterfaceMap->items();

    QComboBox *cbItemId = new QComboBox(this);

    for (auto item : items)
        cbItemId->addItem(QString("%1 - %2").arg(item->name()).arg(item->id()), item->id());

    cbItemId->setEditable(true);
    cbItemId->lineEdit()->setReadOnly(true);
    cbItemId->lineEdit()->setAlignment(Qt::AlignCenter);
    cbItemId->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    cbItemId->installEventFilter(this);

    if (itemId == -1)
        cbItemId->setCurrentIndex(0);
    else
    {
        for (int i = 0; i < cbItemId->count(); ++i)
        {
            if (cbItemId->itemData(i, Qt::UserRole).toInt() == itemId)
            {
                cbItemId->setCurrentIndex(i);
                break;
            }
        }
    }

    ui->tableWidgetNpcLoot->setCellWidget(row, 1, cbItemId);

    QComboBox *cbItemRarity = new QComboBox(this);
    cbItemRarity->addItem("100%", static_cast<int>(QMUD::ItemRarityType::RARITY0));
    cbItemRarity->addItem("50%",  static_cast<int>(QMUD::ItemRarityType::RARITY1));
    cbItemRarity->addItem("20%",  static_cast<int>(QMUD::ItemRarityType::RARITY2));
    cbItemRarity->addItem("10%",  static_cast<int>(QMUD::ItemRarityType::RARITY3));
    cbItemRarity->addItem("5%",   static_cast<int>(QMUD::ItemRarityType::RARITY4));
    cbItemRarity->addItem("2.5%", static_cast<int>(QMUD::ItemRarityType::RARITY5));
    cbItemRarity->addItem("1.25%",static_cast<int>(QMUD::ItemRarityType::RARITY6));
    cbItemRarity->addItem("0.5%", static_cast<int>(QMUD::ItemRarityType::RARITY7));
    cbItemRarity->addItem("0.2%", static_cast<int>(QMUD::ItemRarityType::RARITY8));
    cbItemRarity->addItem("0.1%", static_cast<int>(QMUD::ItemRarityType::RARITY9));
    cbItemRarity->addItem("0.01%",static_cast<int>(QMUD::ItemRarityType::RARITY10));

    cbItemRarity->setEditable(true);
    cbItemRarity->lineEdit()->setReadOnly(true);
    cbItemRarity->lineEdit()->setAlignment(Qt::AlignCenter);
    cbItemRarity->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    cbItemRarity->installEventFilter(this);

    for (int i = 0; i < cbItemRarity->count(); ++i)
    {
        if (cbItemRarity->itemData(i, Qt::UserRole).toInt() == static_cast<int>(rarity))
        {
            cbItemRarity->setCurrentIndex(i);
            break;
        }
    }

    ui->tableWidgetNpcLoot->setCellWidget(row, 2, cbItemRarity);

    QPushButton *pbItemRemove = new QPushButton("Rimuovi", this);
    pbItemRemove->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    connect(pbItemRemove, &QPushButton::clicked, [this, pbItemRemove]() {
        for (auto i = 0; i<ui->tableWidgetNpcLoot->rowCount(); ++i)
        {
            if (qobject_cast<QPushButton*>(ui->tableWidgetNpcLoot->cellWidget(i, 0)) == pbItemRemove)
            {
                ui->tableWidgetNpcLoot->removeRow(i);
                return;
            }
        }
    });

    ui->tableWidgetNpcLoot->setCellWidget(row, 0, pbItemRemove);

    ui->tableWidgetNpcLoot->resizeColumnsToContents();
}

void MainWindow::addNpcShopItem(int itemId)
{
    ui->tableWidgetNpcShopItems->setRowCount(ui->tableWidgetNpcShopItems->rowCount() + 1);
    int row = ui->tableWidgetNpcShopItems->rowCount() - 1;

    auto items = m_npcOnInterfaceMap->items();

    QComboBox *cbItemId = new QComboBox(this);

    for (auto item : items)
        cbItemId->addItem(QString("%1 - %2").arg(item->name()).arg(item->id()), item->id());

    cbItemId->setEditable(true);
    cbItemId->lineEdit()->setReadOnly(true);
    cbItemId->lineEdit()->setAlignment(Qt::AlignCenter);
    cbItemId->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    cbItemId->installEventFilter(this);

    if (itemId == -1)
        cbItemId->setCurrentIndex(0);
    else
    {
        for (int i = 0; i < cbItemId->count(); ++i)
        {
            if (cbItemId->itemData(i, Qt::UserRole).toInt() == itemId)
            {
                cbItemId->setCurrentIndex(i);
                break;
            }
        }
    }

    ui->tableWidgetNpcShopItems->setCellWidget(row, 1, cbItemId);

    QPushButton *pbItemRemove = new QPushButton("Rimuovi", this);
    pbItemRemove->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    connect(pbItemRemove, &QPushButton::clicked, [this, pbItemRemove]() {
        for (auto i = 0; i<ui->tableWidgetNpcShopItems->rowCount(); ++i)
        {
            if (qobject_cast<QPushButton*>(ui->tableWidgetNpcShopItems->cellWidget(i, 0)) == pbItemRemove)
            {
                ui->tableWidgetNpcShopItems->removeRow(i);
                return;
            }
        }
    });

    ui->tableWidgetNpcShopItems->setCellWidget(row, 0, pbItemRemove);
    ui->tableWidgetNpcShopItems->resizeColumnToContents(0);
}

void MainWindow::updateNpcName()
{
    Q_ASSERT(m_npcOnInterface);
    Q_ASSERT(m_widgetNpcOnInterface);

    static_cast<TreeWidgetAction*>(ui->treeWidgetMaps->itemWidget(m_widgetNpcOnInterface, 0))->setText(QString("%1 (Id: %2)").arg(ui->lineEditNpcName->text()).arg(ui->spinBoxNpcId->value()));
}

void MainWindow::updateItemName()
{
    Q_ASSERT(m_itemOnInterface);
    Q_ASSERT(m_widgetItemOnInterface);

    static_cast<TreeWidgetAction*>(ui->treeWidgetMaps->itemWidget(m_widgetItemOnInterface, 0))->setText(QString("%1 (Id: %2)").arg(ui->lineEditItemName->text()).arg(ui->spinBoxItemId->value()));
}

bool MainWindow::saveAll()
{
    on_treeWidgetMaps_itemSelectionChanged();

    QStringList errorList;

    for (int i = 0; i < ui->treeWidgetMaps->topLevelItemCount(); ++i)
    {
        auto mapItem = ui->treeWidgetMaps->topLevelItem(i);

        if (mapItem->data(0, Qt::UserRole).value<EditorMap*>()->filePath().simplified().isEmpty())
        {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Seleziona dove salvare la mappa %1").arg(mapItem->data(0, Qt::UserRole).value<EditorMap*>()->name()),
                                                            "",
                                                            tr("Json file (*.json)"));

            if (!fileName.simplified().isEmpty())
                mapItem->data(0, Qt::UserRole).value<EditorMap*>()->setFilePath(fileName);
            else
                continue;
        }

        if (!mapItem->data(0, Qt::UserRole).value<EditorMap*>()->write(m_ptrMapEditor))
            errorList.push_back(mapItem->data(0, Qt::UserRole).value<EditorMap*>()->filePath());
    }

    if (errorList.isEmpty())
    {
        QMessageBox::information(this, tr("Salvataggio completato"), tr("Tutte le mappe sono state salvate correttamente"));
        return true;
    }
    else
    {
        QMessageBox::critical(this, "Errore", tr("Errore durante il salvataggio dei file:\n %1").arg(errorList.join("\n")));
        return false;
    }
}

QVector<EditorMap *> MainWindow::maps() const
{
    QVector<EditorMap*> maps;

    for (int i=0; i<ui->treeWidgetMaps->topLevelItemCount(); ++i)
    {
        auto item = ui->treeWidgetMaps->topLevelItem(i);
        EditorMap* map = item->data(0, Qt::UserRole).value<EditorMap*>();

        maps.push_back(map);
    }

    return maps;
}

void MainWindow::addQuest(const EditorMap::QuestData &data)
{
    ui->tableWidgetQuests->setRowCount(ui->tableWidgetQuests->rowCount() + 1);

    int row = ui->tableWidgetQuests->rowCount() - 1;

    // Azioni
    QPushButton *pbRemove = new QPushButton("", this);
    pbRemove->setIcon(QIcon(":/icons/icons/remove.png"));
    pbRemove->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    pbRemove->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(pbRemove, &QPushButton::clicked, [this, pbRemove, data]() {
        if (QMessageBox::question(this, tr("Attenzione"),
                                  tr("Vuoi rimuovere la quest %1 - %2").arg(data.id).arg(data.name), QMessageBox::Yes | QMessageBox::No)
                == QMessageBox::Yes)
        {
            for (auto i = 0; i<ui->tableWidgetQuests->rowCount(); ++i)
            {
                if (qobject_cast<QPushButton*>(ui->tableWidgetQuests->cellWidget(i, 0)) == pbRemove)
                {
                    ui->tableWidgetQuests->removeRow(i);
                    return;
                }
            }
        }
    });

    ui->tableWidgetQuests->setCellWidget(row, 0, pbRemove);

    // id, livello, nome
    ui->tableWidgetQuests->setItem(row, 1, new QTableWidgetItem(QString::number(data.id)));
    ui->tableWidgetQuests->item(row, 1)->setData(Qt::UserRole, QVariant::fromValue(data));

    ui->tableWidgetQuests->setItem(row, 2, new QTableWidgetItem(QString::number(data.level)));
    ui->tableWidgetQuests->setItem(row, 3, new QTableWidgetItem(data.name));

    ui->tableWidgetQuests->resizeColumnToContents(0);
    ui->tableWidgetQuests->resizeColumnToContents(1);
    ui->tableWidgetQuests->resizeColumnToContents(2);
}

void MainWindow::addTilesMap(const EditorMap::MapTilesData &data)
{
    ui->tableWidgetTilesMaps->setRowCount(ui->tableWidgetTilesMaps->rowCount() + 1);

    int row = ui->tableWidgetTilesMaps->rowCount() - 1;

    // Azioni
    QPushButton *pbRemove = new QPushButton("", this);
    pbRemove->setIcon(QIcon(":/icons/icons/remove.png"));
    pbRemove->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    pbRemove->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(pbRemove, &QPushButton::clicked, [this, pbRemove, data]() {
        if (QMessageBox::question(this, tr("Attenzione"),
                                  tr("Vuoi rimuovere la mappa %1 - %2").arg(data.mapIndex()).arg(data.mapName()), QMessageBox::Yes | QMessageBox::No)
                == QMessageBox::Yes)
        {
            for (auto i = 0; i<ui->tableWidgetTilesMaps->rowCount(); ++i)
            {
                if (qobject_cast<QPushButton*>(ui->tableWidgetTilesMaps->cellWidget(i, 0)) == pbRemove)
                {
                    ui->tableWidgetTilesMaps->removeRow(i);
                    return;
                }
            }
        }
    });

    ui->tableWidgetTilesMaps->setCellWidget(row, 0, pbRemove);

    // id, nome
    ui->tableWidgetTilesMaps->setItem(row, 1, new QTableWidgetItem(QString::number(data.mapIndex())));
    ui->tableWidgetTilesMaps->item(row, 1)->setData(Qt::UserRole, QVariant::fromValue(data));

    ui->tableWidgetTilesMaps->setItem(row, 2, new QTableWidgetItem(data.mapName()));

    ui->tableWidgetTilesMaps->resizeColumnToContents(0);
    ui->tableWidgetTilesMaps->resizeColumnToContents(1);
}

void MainWindow::on_spinBoxMapID_valueChanged(int arg1)
{
    Q_UNUSED(arg1)

    Q_ASSERT(m_mapOnInterface);
    Q_ASSERT(m_widgetMapOnInterface);

    static_cast<TreeWidgetAction*>(ui->treeWidgetMaps->itemWidget(m_widgetMapOnInterface, 0))->setText(QString("%1 (Id: %2)").arg(ui->lineEditMapName->text()).arg(ui->spinBoxMapID->value()));
}

void MainWindow::on_lineEditMapName_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    Q_ASSERT(m_mapOnInterface);
    Q_ASSERT(m_widgetMapOnInterface);

    static_cast<TreeWidgetAction*>(ui->treeWidgetMaps->itemWidget(m_widgetMapOnInterface, 0))->setText(QString("%1 (Id: %2)").arg(ui->lineEditMapName->text()).arg(ui->spinBoxMapID->value()));
}

void MainWindow::on_spinBoxNpcId_valueChanged(int arg1)
{
    Q_UNUSED(arg1)

    updateNpcName();
}

void MainWindow::on_lineEditNpcName_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    updateNpcName();
}

void MainWindow::on_pushButtonSave_clicked()
{
    saveAll();
}

void MainWindow::on_pushButtonOpen_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Seleziona mappe"),
                                                    "",
                                                    tr("Json file (*.json)"));

    if (!files.isEmpty())
    {
        for (auto fileName : files)
        {
            EditorMap *map = new EditorMap(this);
            map->setFilePath(fileName);
            if (!map->read())
            {
                QMessageBox::critical(this, "Errore", tr("Errore durante il caricamento del file %1").arg(fileName));
                delete map;
                return;
            }

            addMapToTree(map);
        }
    }
}

void MainWindow::on_lineEditItemName_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    updateItemName();
}

void MainWindow::on_spinBoxItemId_valueChanged(int arg1)
{
    Q_UNUSED(arg1)

    updateItemName();
}

void MainWindow::on_comboBoxItemType_currentIndexChanged(int index)
{
    if (static_cast<QMUD::ItemType>(ui->comboBoxItemType->itemData(index).toInt()) == QMUD::ItemType::ARMOR)
    {
        ui->groupBoxItemBaseBonus->setVisible(true);
        ui->groupBoxItemBaseStatus->setVisible(true);
        ui->groupBoxItemClassType->setVisible(true);
        ui->stackedWidgetItemSpecificData->setCurrentWidget(ui->pageItemArmor);
    }
    else if (static_cast<QMUD::ItemType>(ui->comboBoxItemType->itemData(index).toInt()) == QMUD::ItemType::WEAPON)
    {
        ui->groupBoxItemBaseBonus->setVisible(true);
        ui->groupBoxItemBaseStatus->setVisible(true);
        ui->groupBoxItemClassType->setVisible(true);
        ui->stackedWidgetItemSpecificData->setCurrentWidget(ui->pageItemWeapon);
    }
    else if (static_cast<QMUD::ItemType>(ui->comboBoxItemType->itemData(index).toInt()) == QMUD::ItemType::TRINKET)
    {
        ui->groupBoxItemBaseBonus->setVisible(true);
        ui->groupBoxItemBaseStatus->setVisible(true);
        ui->groupBoxItemClassType->setVisible(true);
        ui->stackedWidgetItemSpecificData->setCurrentWidget(ui->pageItemTrinket);
    }
    else if (static_cast<QMUD::ItemType>(ui->comboBoxItemType->itemData(index).toInt()) == QMUD::ItemType::FOOD)
    {
        ui->groupBoxItemBaseBonus->setVisible(false);
        ui->groupBoxItemBaseStatus->setVisible(false);
        ui->groupBoxItemClassType->setVisible(false);
        ui->stackedWidgetItemSpecificData->setCurrentWidget(ui->pageItemFood);
    }
    else if (static_cast<QMUD::ItemType>(ui->comboBoxItemType->itemData(index).toInt()) == QMUD::ItemType::DRINK)
    {
        ui->groupBoxItemBaseBonus->setVisible(false);
        ui->groupBoxItemBaseStatus->setVisible(false);
        ui->groupBoxItemClassType->setVisible(false);
        ui->stackedWidgetItemSpecificData->setCurrentWidget(ui->pageItemDrink);
    }
    else if (static_cast<QMUD::ItemType>(ui->comboBoxItemType->itemData(index).toInt()) == QMUD::ItemType::TONIC)
    {
        ui->groupBoxItemBaseBonus->setVisible(true);
        ui->groupBoxItemBaseStatus->setVisible(false);
        ui->groupBoxItemClassType->setVisible(false);
        ui->stackedWidgetItemSpecificData->setCurrentWidget(ui->pageItemTonic);
    }
    else if (static_cast<QMUD::ItemType>(ui->comboBoxItemType->itemData(index).toInt()) == QMUD::ItemType::POTION)
    {
        ui->groupBoxItemBaseBonus->setVisible(false);
        ui->groupBoxItemBaseStatus->setVisible(true);
        ui->groupBoxItemClassType->setVisible(false);
        ui->stackedWidgetItemSpecificData->setCurrentWidget(ui->pageItemPotion);
    }
    else
    {
        ui->groupBoxItemBaseBonus->setVisible(false);
        ui->groupBoxItemBaseStatus->setVisible(false);
        ui->groupBoxItemClassType->setVisible(false);
        ui->stackedWidgetItemSpecificData->setCurrentWidget(ui->pageItemEmpty);
    }
}

void MainWindow::on_pushButtonNpcEventEnter_clicked()
{
    ScriptEditorDialog dlg(this);
    dlg.setText(m_npcOnInterface->eventEnter());

    if (dlg.exec() == QDialog::Accepted)
    {
        toNpcOnInterface();
        m_npcOnInterface->setEventEnter(dlg.text());
        fromNpcOnInterface();
    }
}

void MainWindow::on_pushButtonNpcEventCustom_clicked()
{
    ScriptEditorDialog dlg(this);
    dlg.setText(m_npcOnInterface->eventCustom());

    if (dlg.exec() == QDialog::Accepted)
    {
        toNpcOnInterface();
        m_npcOnInterface->setEventCustom(dlg.text());
        fromNpcOnInterface();
    }
}

void MainWindow::on_pushButtonNpcLootAdd_clicked()
{
    addNpcLootItem();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (qobject_cast<QComboBox*>(obj) && event->type() == QEvent::Wheel)
    {
        return true;
    }

    return QObject::eventFilter(obj, event);
}

void MainWindow::on_pushButtonNpcShopAddItem_clicked()
{
    addNpcShopItem();
}

void MainWindow::on_pushButtonNpcEventAbility_clicked()
{
    ScriptEditorDialog dlg(this);
    dlg.setText(m_npcOnInterface->eventAbility());

    if (dlg.exec() == QDialog::Accepted)
    {
        toNpcOnInterface();
        m_npcOnInterface->setEventAbility(dlg.text());
        fromNpcOnInterface();
    }
}

void MainWindow::on_pushButtonAddQuest_clicked()
{
    Q_ASSERT(m_mapOnInterface);

    toMapOnInterface();

    EditQuestDialog dialog(maps(), m_mapOnInterface, this);

    EditorMap::QuestData data;
    data.id = m_mapOnInterface->unusedQuestId();
    dialog.setData(data);

    if (dialog.exec() == QDialog::Accepted)
        addQuest(dialog.data());
}

void MainWindow::on_tableWidgetQuests_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)

    EditorMap::QuestData questData = ui->tableWidgetQuests->item(row, 1)->data(Qt::UserRole).value<EditorMap::QuestData>();

    EditQuestDialog dialog(maps(), m_mapOnInterface, this);
    dialog.setData(questData);

    if (dialog.exec() == QDialog::Accepted)
    {
        ui->tableWidgetQuests->item(row, 1)->setData(Qt::UserRole, QVariant::fromValue(dialog.data()));
        ui->tableWidgetQuests->item(row, 1)->setText(QString::number(dialog.data().id));
        ui->tableWidgetQuests->item(row, 2)->setText(QString::number(dialog.data().level));
        ui->tableWidgetQuests->item(row, 3)->setText(dialog.data().name);
    }
}

void MainWindow::on_pushButtonAddTilesMap_clicked()
{
    Q_ASSERT(m_mapOnInterface);

    toMapOnInterface();

    auto index = m_mapOnInterface->unusedMapTilesId();

    addTilesMap(EditorMap::MapTilesData(tr("Mappa %1").arg(index), index, QPixmap(), QString(), QString()));
}

void MainWindow::on_tableWidgetTilesMaps_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)

    EditorMap::MapTilesData mapData = ui->tableWidgetTilesMaps->item(row, 1)->data(Qt::UserRole).value<EditorMap::MapTilesData>();

    if (m_ptrMapEditor->setMap(mapData))
        m_ptrMapEditor->show();
}
