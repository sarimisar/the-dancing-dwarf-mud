/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>

#include "editormap.h"

namespace Ui {
class MainWindow;
}

class EditorRoom;
class EditorNpc;
class EditorItem;
class FormTilesMapEditor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void onEditorMapTilesEditCompleted();

    void on_pushButtonNew_clicked();
    void on_treeWidgetMaps_itemSelectionChanged();
    void on_spinBoxMapID_valueChanged(int arg1);
    void on_lineEditMapName_textChanged(const QString &arg1);
    void on_spinBoxNpcId_valueChanged(int arg1);
    void on_lineEditNpcName_textChanged(const QString &arg1);
    void on_pushButtonSave_clicked();
    void on_pushButtonOpen_clicked();
    void on_lineEditItemName_textChanged(const QString &arg1);
    void on_spinBoxItemId_valueChanged(int arg1);
    void on_comboBoxItemType_currentIndexChanged(int index);
    void on_pushButtonNpcEventEnter_clicked();
    void on_pushButtonNpcEventCustom_clicked();
    void on_pushButtonNpcLootAdd_clicked();
    void on_pushButtonNpcShopAddItem_clicked();
    void on_pushButtonNpcEventAbility_clicked();
    void on_pushButtonAddQuest_clicked();
    void on_tableWidgetQuests_cellDoubleClicked(int row, int column);
    void on_pushButtonAddTilesMap_clicked();
    void on_tableWidgetTilesMaps_cellDoubleClicked(int row, int column);

private:
    bool eventFilter(QObject *obj, QEvent *event);

    void addMapToTree(EditorMap *map);
    void addNpcToTree(EditorNpc *npc, QTreeWidgetItem* itemMap);
    void addItemToTree(EditorItem *item, QTreeWidgetItem* itemMap);
    void fromMapOnInterface();
    void toMapOnInterface();
    void fromNpcOnInterface();
    void toNpcOnInterface();
    void fromItemOnInterface();
    void toItemOnInterface();
    EditorMap* findMap(int mapId) const;
    void addNpcLootItem(int itemId = -1, QMUD::ItemRarityType rarity = QMUD::ItemRarityType::RARITY1);
    void addNpcShopItem(int itemId = -1);
    void updateNpcName();
    void updateItemName();
    bool saveAll();
    QVector<EditorMap*> maps() const;
    void addQuest(const EditorMap::QuestData& data);
    void addTilesMap(const EditorMap::MapTilesData& data);

    Ui::MainWindow *ui;

    EditorMap* m_mapOnInterface;
    QTreeWidgetItem* m_widgetMapOnInterface;

    EditorNpc* m_npcOnInterface;
    EditorMap* m_npcOnInterfaceMap;
    QTreeWidgetItem* m_widgetNpcOnInterface;

    EditorItem* m_itemOnInterface;
    EditorMap* m_itemOnInterfaceMap;
    QTreeWidgetItem* m_widgetItemOnInterface;

    FormTilesMapEditor* m_ptrMapEditor;
};

#endif // MAINWINDOW_H
