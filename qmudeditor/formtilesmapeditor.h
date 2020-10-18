#ifndef FORMTILESMAPEDITOR_H
#define FORMTILESMAPEDITOR_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QUndoStack>
#include <QUndoCommand>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QRandomGenerator>
#include <QTableWidgetItem>

#include "editexitdialog.h"
#include "editormap.h"

namespace Ui {
class FormTilesMapEditor;
}

class FlagsMap
{
public:
    FlagsMap() { }
    ~FlagsMap() { }

    FlagsMap(int width, int height);
    FlagsMap(const QSize& size);
    FlagsMap(const FlagsMap &other);

    void operator=(const FlagsMap& other);

    int width() const;
    int height() const;

    bool at(int x, int y) const;
    void setAt(int x, int y, bool value);

    QString toString() const;
    bool fromString(QString str, int width, int height);

private:
    QVector<QVector<bool>> m_data;
};

Q_DECLARE_METATYPE(FlagsMap);

class MyGraphicsItem : public QGraphicsPixmapItem
{
public:
    MyGraphicsItem(const QPixmap &pixmap, QUndoStack* undoStack, QWidget* parentWnd, QGraphicsItem *parent = nullptr);
    virtual ~MyGraphicsItem() override { }

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    virtual int type() const override;

private:
    QUndoStack* m_ptrUndoStack;

    QWidget* m_ptrParentWnd;

    QPointF m_oldPos;
};

class MyGraphicsImageItem : public QGraphicsItem
{
public:
    MyGraphicsImageItem(const QPixmap &pixmap, const FlagsMap& rooms, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    QPixmap &pixmap();
    FlagsMap& rooms();
    FlagsMap rooms() const;

private:
    QPixmap m_pixmap;
    FlagsMap m_vRooms;
};

class ImageDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class FormTilesMapEditor : public QWidget
{
    friend class MyGraphicsItem;

    Q_OBJECT

public:
    explicit FormTilesMapEditor(QWidget *parent = nullptr);
    virtual ~FormTilesMapEditor() override;

    bool writeMapData(const EditorMap::MapTilesData& map, QJsonObject& obj, QVector<QJsonObject> &roomsObjs);
    bool writeMapImages(const EditorMap::MapTilesData& map, QString strBaseName);

    void setNpcs(QVector<EditorNpc *> npcs);

    bool setMap(const EditorMap::MapTilesData &map);
    bool getMap(EditorMap::MapTilesData& map);

signals:
    void editComplete();

private slots:
    void onActionTriggered(bool checked);
    void onRubberBandChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint);

    void on_tableWidgetExits_itemSelectionChanged();
    void on_tableWidgetTrees_itemSelectionChanged();
    void on_tableWidgetBuildings_itemSelectionChanged();
    void on_tableWidgetTerrains_itemSelectionChanged();
    void on_tableWidgetNpcs_itemSelectionChanged();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void updateGrid();
    void loadResources();
    void updateActionsStatus(QObject *obj);
    void clearSelection(QObject *excluded = Q_NULLPTR);
    MyGraphicsItem *createExit(QPixmap pixmap, QString fileName, int filePixmapIndex, const EditExitDialog::ExitData& exitData);
    MyGraphicsItem *createTree(QPixmap pixmap, const FlagsMap& map, QString fileName, int filePixmapIndex);
    MyGraphicsItem *createBuilding(QPixmap pixmap, const FlagsMap &map, QString fileName, int filePixmapIndex);
    MyGraphicsItem *createNpc(QPixmap pixmap, QString fileName, int filePixmapIndex);

    Ui::FormTilesMapEditor *ui;

    mutable QUndoStack m_undoStack;

    bool m_bResourcesLoaded;

    QAction* m_ptrRectangularSelection;
    QAction* m_ptrDrawTerrainRect;
    QAction* m_ptrRubber;
    QGraphicsScene m_sceneMap;
    QGraphicsScene m_sceneMapRemovedItems;
    QVector<QGraphicsLineItem*> m_vLineItems;

    struct CurrentSelection
    {
        enum class Type
        {
            NONE,
            EXIT,
            TERRAIN,
            TREE,
            BUILDING,
            NPC
        };

        CurrentSelection() :
            type(Type::NONE)
        { }

        static Type typeFromString(QString type)
        {
            if (type.compare("EXIT", Qt::CaseInsensitive) == 0)
                return Type::EXIT;
            else if (type.compare("TERRAIN", Qt::CaseInsensitive) == 0)
                return Type::TERRAIN;
            else if (type.compare("TREE", Qt::CaseInsensitive) == 0)
                return Type::TREE;
            else if (type.compare("BUILDING", Qt::CaseInsensitive) == 0)
                return Type::BUILDING;
            else if (type.compare("NPC", Qt::CaseInsensitive) == 0)
                return Type::NPC;
            else
                return Type::NONE;
        }

        static QString typeToString(Type type)
        {
            if (type == Type::EXIT)
                return "EXIT";
            else if (type == Type::TERRAIN)
                return "TERRAIN";
            else if (type == Type::TREE)
                return "TREE";
            else if (type == Type::BUILDING)
                return "BUILDING";
            else if (type == Type::NPC)
                return "NPC";
            else
                return "NONE";
        }

        Type type;
        QPixmap image;
        FlagsMap rooms;
    };

    struct ObjectData
    {
        CurrentSelection::Type type;
        QString filename;
        int index;
        int x;
        int y;

        QVariant specificData;
    };

    static QVector<ObjectData> objectDataListFromString(QString str);
    QString objectDataListToString();

    CurrentSelection m_currentSelection;
    MyGraphicsItem* m_ptrCurrentSelectionItem;

    MyGraphicsImageItem* m_ptrPixmapTerrainItem;

    QPointF m_lastTerrainPos;

    QRandomGenerator m_randomGenerator;

    int m_iMapIndex;
};

class InsertItemCommand : public QUndoCommand
{
public:
    InsertItemCommand(QGraphicsItem* item, QGraphicsScene* scene, QPointF pos);

    void redo() override;
    void undo() override;

private:
    QGraphicsItem *m_item;
    QGraphicsScene* m_scene;
    QPointF m_pos;
};

class RemoveItemsCommand : public QUndoCommand
{
public:
    RemoveItemsCommand(QList<QGraphicsItem*> items, QGraphicsScene* scene, QGraphicsScene* sceneRemovedItems);

    void redo() override;
    void undo() override;

private:
    QList<QGraphicsItem*> m_items;
    QGraphicsScene* m_scene;
    QGraphicsScene* m_sceneRemovedItems;
};

class InsertTerrainCommand : public QUndoCommand
{
public:
    InsertTerrainCommand(QTableWidgetItem* selectedItem, MyGraphicsImageItem* item, QPoint pos);

    void redo() override;
    void undo() override;

private:
    QTableWidgetItem* m_selectedItem;

    QPixmap m_oldPixmap;
    bool m_bOldIsRoom;
    MyGraphicsImageItem* m_item;
    QPoint m_pos;
};

class DeleteTerrainCommand : public QUndoCommand
{
public:
    DeleteTerrainCommand(MyGraphicsImageItem* item, QPoint pos);

    void redo() override;
    void undo() override;

private:
    QPixmap m_oldPixmap;
    bool m_bOldIsRoom;
    MyGraphicsImageItem* m_item;
    QPoint m_pos;
};

class InsertRectTerrainCommand : public QUndoCommand
{
public:
    InsertRectTerrainCommand(QList<QTableWidgetItem*> selectedItems, MyGraphicsImageItem* item, QPointF from, QPointF to, QRandomGenerator& randomGenerator);

    void redo() override;
    void undo() override;

private:
    QPixmap m_oldPixmap;
    FlagsMap m_vOldIsRooms;

    QPointF m_fromScenePoint;
    QPointF m_toScenePoint;

    QList<QTableWidgetItem*> m_selectedItems;
    QRandomGenerator& m_randomGenerator;
    MyGraphicsImageItem* m_item;
};

class MoveItemCommand : public QUndoCommand
{
public:
    MoveItemCommand(MyGraphicsItem* item, QPointF newPos, QPointF oldPos);

    void redo() override;
    void undo() override;

private:
    bool m_bFirstRedo;

    MyGraphicsItem *m_item;
    QPointF m_newPos;
    QPointF m_oldPos;
};

#endif // FORMTILESMAPEDITOR_H
