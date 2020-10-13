#include "formtilesmapeditor.h"
#include "ui_formtilesmapeditor.h"

#include "graphicsviewzoom.h"
#include "editexitdialog.h"

#include <QGraphicsLineItem>
#include <QDir>
#include <QFileInfo>
#include <QMap>
#include <QRgb>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QAction>
#include <QToolBar>
#include <QRandomGenerator>
#include <QOpenGLWidget>
#include <QGraphicsDropShadowEffect>
#include <QRandomGenerator>
#include <QJsonArray>
#include <QJsonObject>

#define MAP_TILE_SIZE_PX            80
#define MAP_SIZE_WIDTH              50
#define MAP_SIZE_HEIGHT             50

#define Z_VALUE_TERRAIN             0
#define Z_VALUE_TREE                1
#define Z_VALUE_BUILDING            2
#define Z_VALUE_EXITS               3
#define Z_VALUE_CURRENT_SELECTION   4
#define Z_VALUE_GRID                5

#define USER_ROLE_PIXMAP                    (Qt::UserRole + 0)
#define USER_ROLE_ROOMS                     (Qt::UserRole + 1)
#define USER_ROLE_FILE_NAME                 (Qt::UserRole + 2)
#define USER_ROLE_FILE_PIXMAP_INDEX         (Qt::UserRole + 3)
#define USER_ROLE_TYPE                      (Qt::UserRole + 4)
#define USER_ROLE_EXIT_DATA                 (Qt::UserRole + 5)

#define USER_TYPE_MY_GRAPHICS_ITEM          (QTableWidgetItem::UserType)

FlagsMap::FlagsMap(int width, int height)
{
    m_data.resize(height);
    for (auto& row : m_data)
        row.resize(width);
}

FlagsMap::FlagsMap(const QSize &size)
{
    m_data.resize(size.height());
    for (auto& row : m_data)
        row.resize(size.width());
}

FlagsMap::FlagsMap(const FlagsMap &other)
{
    m_data = other.m_data;
}

void FlagsMap::operator=(const FlagsMap &other)
{
    m_data = other.m_data;
}

int FlagsMap::width() const
{
    return m_data.isEmpty() ? 0 : m_data[0].size();
}

int FlagsMap::height() const
{
    return m_data.size();
}

bool FlagsMap::at(int x, int y) const
{
    if (x >= 0 && x < width() &&
            y >=0 && y < height())
    {
        return m_data[y][x];
    }

    return false;
}

void FlagsMap::setAt(int x, int y, bool value)
{
    if (x >= 0 && x < width() &&
            y >=0 && y < height())
    {
        m_data[y][x] = value;
    }
}

QString FlagsMap::toString() const
{
    QString ret;

    for (int y = 0; y < height(); ++y)
    {
        for (int x = 0; x < width(); ++x)
        {
            if (x != 0)
                ret += ",";

            ret += m_data[y][x] ? "1" : "0";
        }

        if (y < height() - 1)
            ret += "\n";
    }

    return ret;
}

bool FlagsMap::fromString(QString str, int width, int height)
{
    m_data.resize(height);
    for (auto& row : m_data)
    {
        row.resize(width);
        for (auto& elem : row)
            elem = false;
    }

    if (str.isEmpty())
        return true;

    QStringList lines = str.split("\n");

    if (lines.size() != height)
        return false;

    for (int y = 0; y < height; ++y)
    {
        QString line = lines[y];

        if (line.simplified().isEmpty())
            continue;

        auto columns = line.split(',');

        if (columns.size() != width)
            return false;

        for (int x = 0; x < width; ++x)
            m_data[y][x] = (columns[x].toInt() == 1);
    }

    return true;
}

FormTilesMapEditor::FormTilesMapEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTilesMapEditor),
    m_bResourcesLoaded(false),
    m_ptrCurrentSelectionItem(Q_NULLPTR),
    m_iMapIndex(-1)
{
    ui->setupUi(this);

    // Creo la toolbar
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setOrientation(Qt::Vertical);
    ui->horizontalLayoutToolbar->addWidget(toolBar);

    m_ptrRectangularSelection = new QAction(tr("Selezione rettangolare"));
    m_ptrRectangularSelection->setCheckable(true);
    m_ptrRectangularSelection->setChecked(false);
    m_ptrRectangularSelection->setIcon(QIcon(":/icons/icons/rectangular_selection.svg"));
    connect(m_ptrRectangularSelection, &QAction::triggered, this, &FormTilesMapEditor::onActionTriggered);
    toolBar->addAction(m_ptrRectangularSelection);

    m_ptrDrawTerrainRect = new QAction(tr("Disegna rettangolo"));
    m_ptrDrawTerrainRect->setCheckable(true);
    m_ptrDrawTerrainRect->setChecked(false);
    m_ptrDrawTerrainRect->setIcon(QIcon(":/icons/icons/rectangular_draw.svg"));
    connect(m_ptrDrawTerrainRect, &QAction::triggered, this, &FormTilesMapEditor::onActionTriggered);
    toolBar->addAction(m_ptrDrawTerrainRect);

    m_ptrRubber = new QAction(tr("Cancella"));
    m_ptrRubber->setCheckable(true);
    m_ptrRubber->setChecked(false);
    m_ptrRubber->setIcon(QIcon(":/icons/icons/rubber.svg"));
    connect(m_ptrRubber, &QAction::triggered, this, &FormTilesMapEditor::onActionTriggered);
    toolBar->addAction(m_ptrRubber);

    // Inizializzo lo stack undo/redo
    m_undoStack.setUndoLimit(100);

    QAction* undoAction = m_undoStack.createUndoAction(this, tr("&Undo"));
    QAction* redoAction = m_undoStack.createRedoAction(this, tr("&Redo"));

    undoAction->setShortcut(QKeySequence::Undo);
    redoAction->setShortcut(QKeySequence::Redo);

    undoAction->setIcon(QIcon(":/icons/icons/undo.png"));
    redoAction->setIcon(QIcon(":/icons/icons/redo.png"));

    toolBar->addAction(undoAction);
    toolBar->addAction(redoAction);

    // Inizializzo la scena
    m_sceneMap.setSceneRect(0, 0, 1 * MAP_TILE_SIZE_PX, 1 * MAP_TILE_SIZE_PX);
    m_sceneMap.installEventFilter(this);

    // Inizializzo la pixmap del terreno
    QPixmap pixmapTerrain = QPixmap(1 * MAP_TILE_SIZE_PX, 1 * MAP_TILE_SIZE_PX);
    pixmapTerrain.fill(QColor(0, 0, 0, 0));

    FlagsMap roomsTerrain(MAP_SIZE_WIDTH, MAP_SIZE_HEIGHT);

    m_ptrPixmapTerrainItem = new MyGraphicsImageItem(pixmapTerrain, roomsTerrain);
    m_ptrPixmapTerrainItem->setPos(0, 0);
    m_ptrPixmapTerrainItem->setZValue(Z_VALUE_TERRAIN);
    m_sceneMap.addItem(m_ptrPixmapTerrainItem);

    // Inizializzo widget
    GraphicsViewZoom* gvz = new GraphicsViewZoom(ui->graphicsView);
    gvz->setModifiers(Qt::NoModifier);

    //ui->graphicsView->setViewport(new QOpenGLWidget());
    ui->graphicsView->setScene(&m_sceneMap);
    ui->graphicsView->setSceneRect(m_sceneMap.sceneRect());
    ui->graphicsView->installEventFilter(this);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    connect(ui->graphicsView, &QGraphicsView::rubberBandChanged, this, &FormTilesMapEditor::onRubberBandChanged);

    ui->tableWidgetExits->setItemDelegate(new ImageDelegate());
    ui->tableWidgetTrees->setItemDelegate(new ImageDelegate());
    ui->tableWidgetBuildings->setItemDelegate(new ImageDelegate());
    ui->tableWidgetTerrains->setItemDelegate(new ImageDelegate());

    // Genero la griglia
    updateGrid();

    // Carico le risorse
    loadResources();
}

FormTilesMapEditor::~FormTilesMapEditor()
{
    delete ui;
}

bool FormTilesMapEditor::writeMapData(const EditorMap::MapTilesData &map, QJsonObject &obj, QVector<QJsonObject> &roomsObjs)
{
    // Scrivo la flag rooms
    FlagsMap roomsFlags;

    if (!roomsFlags.fromString(map.terrain(), MAP_SIZE_WIDTH, MAP_SIZE_HEIGHT))
        return false;

    auto objects = objectDataListFromString(map.objects());

    for (int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].type == CurrentSelection::Type::TREE)
        {
            QTableWidgetItem *item = Q_NULLPTR;

            for (int row = 0; row < ui->tableWidgetTrees->rowCount() && !item; ++row)
            {
                for (int column = 0; column < ui->tableWidgetTrees->columnCount() && !item; ++column)
                {
                    auto tmpItem = ui->tableWidgetTrees->item(row, column);

                    if (tmpItem &&
                            tmpItem->data(USER_ROLE_FILE_NAME).toString() == objects[i].filename &&
                            tmpItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt() == objects[i].index)
                    {
                        item = tmpItem;
                    }
                }
            }

            if (item)
            {
                FlagsMap roomsMap = item->data(USER_ROLE_ROOMS).value<FlagsMap>();

                for (int x = 0; x < roomsMap.width(); ++x)
                    for (int y = 0; y < roomsMap.height(); ++y)
                    {
                        if (!roomsMap.at(x, y))
                            roomsFlags.setAt(objects[i].x + x, objects[i].y + y, false);
                    }
            }
        }
        else if (objects[i].type == CurrentSelection::Type::BUILDING)
        {
            QTableWidgetItem *item = Q_NULLPTR;

            for (int row = 0; row < ui->tableWidgetBuildings->rowCount() && !item; ++row)
            {
                for (int column = 0; column < ui->tableWidgetBuildings->columnCount() && !item; ++column)
                {
                    auto tmpItem = ui->tableWidgetBuildings->item(row, column);

                    if (tmpItem &&
                            tmpItem->data(USER_ROLE_FILE_NAME).toString() == objects[i].filename &&
                            tmpItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt() == objects[i].index)
                    {
                        item = tmpItem;
                    }
                }
            }

            if (item)
            {
                FlagsMap roomsMap = item->data(USER_ROLE_ROOMS).value<FlagsMap>();

                for (int x = 0; x < roomsMap.width(); ++x)
                    for (int y = 0; y < roomsMap.height(); ++y)
                    {
                        if (!roomsMap.at(x, y))
                            roomsFlags.setAt(objects[i].x + x, objects[i].y + y, false);
                    }
            }
        }
    }

    QJsonArray arrayRooms;

    for (int y = 0; y < MAP_SIZE_HEIGHT; ++y)
    {
        QString row;

        for (int x = 0; x < MAP_SIZE_WIDTH; ++x)
            row += (roomsFlags.at(x, y)) ? "1" : "0";

        arrayRooms.push_back(row);
    }

    obj["rooms"] = arrayRooms;

    // Uscite
    QMap<QString, QJsonObject> rooms;

    for (int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].type == CurrentSelection::Type::EXIT)
        {
            QTableWidgetItem *item = Q_NULLPTR;

            for (int row = 0; row < ui->tableWidgetExits->rowCount() && !item; ++row)
            {
                for (int column = 0; column < ui->tableWidgetExits->columnCount() && !item; ++column)
                {
                    auto tmpItem = ui->tableWidgetExits->item(row, column);

                    if (tmpItem &&
                            tmpItem->data(USER_ROLE_FILE_NAME).toString() == objects[i].filename &&
                            tmpItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt() == objects[i].index)
                    {
                        item = tmpItem;
                    }
                }
            }

            if (item)
            {
                QString name = item->data(USER_ROLE_FILE_NAME).toString();
                int mapIndex = map.mapIndex();
                int x = objects[i].x;
                int y = objects[i].y;
                QString key = QString("%1-%2-%3").arg(x).arg(y).arg(mapIndex);

                auto exitData = objects[i].specificData.value<EditExitDialog::ExitData>();

                QJsonObject exitObj;

                exitObj["code"] = exitData.code;

                if (exitData.action != QMUD::ActionType::UNKNOWN)
                {
                    exitObj["openAction"] = QMUD::actionToString(exitData.action);
                    exitObj["closeAction"] = exitData.action == QMUD::ActionType::OPEN ? QMUD::actionToString(QMUD::ActionType::CLOSE) :
                                                                                        QMUD::actionToString(exitData.action);
                }

                if (exitData.isOpen)
                    exitObj["isOpen"] = exitData.isOpen;

                if (exitData.isHidden)
                    exitObj["isHidden"] = exitData.isHidden;

                if (!exitData.descOpen.isEmpty())
                    exitObj["descOpen"] = exitData.descOpen;

                if (!exitData.descClose.isEmpty())
                    exitObj["descClose"] = exitData.descClose;

                if (!exitData.descOpenOther.isEmpty())
                    exitObj["descOpenOther"] = exitData.descOpenOther;

                if (!exitData.descCloseOther.isEmpty())
                    exitObj["descCloseOther"] = exitData.descCloseOther;

                if (!exitData.door.isEmpty())
                    exitObj["door"] = exitData.door;

                rooms[key][name] = exitObj;
            }
        }
    }

    // Scrivo le stanze
    for (auto it = rooms.begin(); it != rooms.end(); ++it)
    {
        QJsonObject objRoom = it.value();

        QStringList list = it.key().split("-");
        QJsonArray roomCoord;
        roomCoord.push_back(list[0].toInt());
        roomCoord.push_back(list[1].toInt());
        roomCoord.push_back(list[2].toInt());
        objRoom["roomCoord"] = roomCoord;
        objRoom["name"] = "";

        roomsObjs.push_back(objRoom);
    }

    return true;
}

bool FormTilesMapEditor::writeMapImages(const EditorMap::MapTilesData &map, QString strBaseName)
{
    QString mapLevel0 = strBaseName + QString("_%1_0.png").arg(map.mapIndex());
    QString mapLevel1 = strBaseName + QString("_%1_1.png").arg(map.mapIndex());

    // Salvo il livello 0
    map.pixmapTerrain().save(mapLevel0);

    // Salvo il livello 1
    QGraphicsScene sceneLevel1;
    sceneLevel1.setSceneRect(0, 0, MAP_SIZE_WIDTH * MAP_TILE_SIZE_PX, MAP_SIZE_HEIGHT * MAP_TILE_SIZE_PX);
    sceneLevel1.setBackgroundBrush(QColor(0, 0, 0, 0));

    auto objects = objectDataListFromString(map.objects());

    for (int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].type == CurrentSelection::Type::TREE)
        {
            QTableWidgetItem *item = Q_NULLPTR;

            for (int row = 0; row < ui->tableWidgetTrees->rowCount() && !item; ++row)
            {
                for (int column = 0; column < ui->tableWidgetTrees->columnCount() && !item; ++column)
                {
                    auto tmpItem = ui->tableWidgetTrees->item(row, column);

                    if (tmpItem &&
                            tmpItem->data(USER_ROLE_FILE_NAME).toString() == objects[i].filename &&
                            tmpItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt() == objects[i].index)
                    {
                        item = tmpItem;
                    }
                }
            }

            if (item)
            {
                QPixmap pix = item->data(USER_ROLE_PIXMAP).value<QPixmap>();

                auto newItem = createTree(pix, FlagsMap(), "", 0);
                newItem->setPos(objects[i].x * MAP_TILE_SIZE_PX, objects[i].y * MAP_TILE_SIZE_PX);
                sceneLevel1.addItem(newItem);
            }
        }
        else if (objects[i].type == CurrentSelection::Type::BUILDING)
        {
            QTableWidgetItem *item = Q_NULLPTR;

            for (int row = 0; row < ui->tableWidgetBuildings->rowCount() && !item; ++row)
            {
                for (int column = 0; column < ui->tableWidgetBuildings->columnCount() && !item; ++column)
                {
                    auto tmpItem = ui->tableWidgetBuildings->item(row, column);

                    if (tmpItem &&
                            tmpItem->data(USER_ROLE_FILE_NAME).toString() == objects[i].filename &&
                            tmpItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt() == objects[i].index)
                    {
                        item = tmpItem;
                    }
                }
            }

            if (item)
            {
                QPixmap pix = item->data(USER_ROLE_PIXMAP).value<QPixmap>();

                auto newItem = createTree(pix, FlagsMap(), "", 0);
                newItem->setPos(objects[i].x * MAP_TILE_SIZE_PX, objects[i].y * MAP_TILE_SIZE_PX);
                sceneLevel1.addItem(newItem);
            }
        }
    }

    QPixmap pix(sceneLevel1.sceneRect().size().toSize());
    pix.fill(QColor(0, 0, 0, 0));
    QPainter painter(&pix);
    sceneLevel1.render(&painter);
    painter.end();

    pix.save(mapLevel1);

    return true;
}

bool FormTilesMapEditor::setMap(const EditorMap::MapTilesData& map)
{
    // Carico il terreno
    QRect rectTerrainTiles;

    QPixmap pixmapTerrain = map.pixmapTerrain();
    FlagsMap terrain;

    if (pixmapTerrain.isNull())
    {
        pixmapTerrain = QPixmap(MAP_SIZE_WIDTH * MAP_TILE_SIZE_PX, MAP_SIZE_HEIGHT * MAP_TILE_SIZE_PX);
        pixmapTerrain.fill(QColor(0, 0, 0, 0));
    }

    {
        if (pixmapTerrain.width() > MAP_SIZE_WIDTH * MAP_TILE_SIZE_PX ||
            pixmapTerrain.height() > MAP_SIZE_HEIGHT * MAP_TILE_SIZE_PX ||
            pixmapTerrain.width() % MAP_TILE_SIZE_PX != 0 ||
            pixmapTerrain.height() % MAP_TILE_SIZE_PX != 0)
        {
            return false;
        }

        int widthMap = pixmapTerrain.width() / MAP_TILE_SIZE_PX;
        int heightMap = pixmapTerrain.height() / MAP_TILE_SIZE_PX;

        rectTerrainTiles = QRect((MAP_SIZE_WIDTH - widthMap)/2,
                        (MAP_SIZE_HEIGHT - heightMap)/2,
                        widthMap,
                        heightMap);

        if (!terrain.fromString(map.terrain(), MAP_SIZE_WIDTH, MAP_SIZE_HEIGHT))
            return false;
    }


    // Carico gli oggetti
    QVector<ObjectData> objects = objectDataListFromString(map.objects());

    // Pulisco lo stack undo redo
    m_undoStack.clear();

    // Pulisco la scena
    m_sceneMap.clear();
    m_vLineItems.clear();

    m_currentSelection = CurrentSelection();
    m_ptrCurrentSelectionItem = Q_NULLPTR;
    m_ptrPixmapTerrainItem = Q_NULLPTR;
    m_lastTerrainPos = QPointF();

    // Inizializzo la scena
    m_sceneMap.setSceneRect(0, 0, MAP_SIZE_WIDTH * MAP_TILE_SIZE_PX, MAP_SIZE_HEIGHT * MAP_TILE_SIZE_PX);

    // Inizializzo la pixmap del terreno
    QPixmap pixmap(MAP_SIZE_WIDTH * MAP_TILE_SIZE_PX, MAP_SIZE_HEIGHT * MAP_TILE_SIZE_PX);
    pixmap.fill(QColor(0, 0, 0, 0));

    // Aggiungo il terreno
    {
        QPainter painter(&pixmap);
        painter.drawPixmap(rectTerrainTiles.topLeft() * MAP_TILE_SIZE_PX, pixmapTerrain);
        painter.end();
    }

    m_ptrPixmapTerrainItem = new MyGraphicsImageItem(pixmap, terrain);
    m_ptrPixmapTerrainItem->setPos(0, 0);
    m_ptrPixmapTerrainItem->setZValue(Z_VALUE_TERRAIN);
    m_sceneMap.addItem(m_ptrPixmapTerrainItem);

    // Aggiungo gli oggetti
    for (int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].type == CurrentSelection::Type::EXIT)
        {
            QTableWidgetItem *item = Q_NULLPTR;

            for (int row = 0; row < ui->tableWidgetExits->rowCount() && !item; ++row)
            {
                for (int column = 0; column < ui->tableWidgetExits->columnCount() && !item; ++column)
                {
                    auto tmpItem = ui->tableWidgetExits->item(row, column);

                    if (tmpItem &&
                            tmpItem->data(USER_ROLE_FILE_NAME).toString() == objects[i].filename &&
                            tmpItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt() == objects[i].index)
                    {
                        item = tmpItem;
                    }
                }
            }

            if (item)
            {
                MyGraphicsItem* newItem = createExit(item->data(USER_ROLE_PIXMAP).value<QPixmap>(),
                                                     item->data(USER_ROLE_FILE_NAME).toString(),
                                                     item->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt(),
                                                     objects[i].specificData.value<EditExitDialog::ExitData>());

                newItem->setPos(QPointF(objects[i].x * MAP_TILE_SIZE_PX, objects[i].y * MAP_TILE_SIZE_PX));

                m_sceneMap.addItem(newItem);
            }
        }
        else if (objects[i].type == CurrentSelection::Type::TREE)
        {
            QTableWidgetItem *item = Q_NULLPTR;

            for (int row = 0; row < ui->tableWidgetTrees->rowCount() && !item; ++row)
            {
                for (int column = 0; column < ui->tableWidgetTrees->columnCount() && !item; ++column)
                {
                    auto tmpItem = ui->tableWidgetTrees->item(row, column);

                    if (tmpItem &&
                            tmpItem->data(USER_ROLE_FILE_NAME).toString() == objects[i].filename &&
                            tmpItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt() == objects[i].index)
                    {
                        item = tmpItem;
                    }
                }
            }

            if (item)
            {
                MyGraphicsItem* newItem = createTree(item->data(USER_ROLE_PIXMAP).value<QPixmap>(),
                                                     item->data(USER_ROLE_ROOMS).value<FlagsMap>(),
                                                     item->data(USER_ROLE_FILE_NAME).toString(),
                                                     item->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt());

                newItem->setPos(QPointF(objects[i].x * MAP_TILE_SIZE_PX, objects[i].y * MAP_TILE_SIZE_PX));

                m_sceneMap.addItem(newItem);
            }
        }
        else if (objects[i].type == CurrentSelection::Type::BUILDING)
        {
            QTableWidgetItem *item = Q_NULLPTR;

            for (int row = 0; row < ui->tableWidgetBuildings->rowCount() && !item; ++row)
            {
                for (int column = 0; column < ui->tableWidgetBuildings->columnCount() && !item; ++column)
                {
                    auto tmpItem = ui->tableWidgetBuildings->item(row, column);

                    if (tmpItem &&
                            tmpItem->data(USER_ROLE_FILE_NAME).toString() == objects[i].filename &&
                            tmpItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt() == objects[i].index)
                    {
                        item = tmpItem;
                    }
                }
            }

            if (item)
            {
                MyGraphicsItem* newItem = createBuilding(item->data(USER_ROLE_PIXMAP).value<QPixmap>(),
                                                     item->data(USER_ROLE_ROOMS).value<FlagsMap>(),
                                                     item->data(USER_ROLE_FILE_NAME).toString(),
                                                     item->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt());

                newItem->setPos(QPointF(objects[i].x * MAP_TILE_SIZE_PX, objects[i].y * MAP_TILE_SIZE_PX));

                m_sceneMap.addItem(newItem);
            }
        }
    }

    // Inizializzo gli widgets
    ui->graphicsView->setSceneRect(m_sceneMap.sceneRect());
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    m_iMapIndex = map.mapIndex();
    ui->lineEditMapName->setText(map.mapName());

    // Genero la griglia
    updateGrid();

    return true;
}

bool FormTilesMapEditor::getMap(EditorMap::MapTilesData &map)
{
    map = EditorMap::MapTilesData(ui->lineEditMapName->text(),
                                  m_iMapIndex,
                                  m_ptrPixmapTerrainItem->pixmap(),
                                  m_ptrPixmapTerrainItem->rooms().toString(),
                                  objectDataListToString());

    return true;
}

void FormTilesMapEditor::onActionTriggered(bool checked)
{
    Q_UNUSED(checked)

    updateActionsStatus(sender());
}

void FormTilesMapEditor::onRubberBandChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint)
{
    static QPointF lastFromScenePoint = fromScenePoint;
    static  QPointF lastToScenePoint = toScenePoint;

    if (m_ptrDrawTerrainRect->isChecked() && !rubberBandRect.isNull())
    {
        lastFromScenePoint = fromScenePoint;
        lastToScenePoint = toScenePoint;
    }

    if (m_ptrDrawTerrainRect->isChecked() && rubberBandRect.isNull())
    {
        if (m_ptrCurrentSelectionItem)
        {
            if (m_currentSelection.type == CurrentSelection::Type::TERRAIN)
            {
                auto selectedItems = ui->tableWidgetTerrains->selectedItems();

                if (!selectedItems.isEmpty())
                {
                    fromScenePoint = lastFromScenePoint;
                    toScenePoint = lastToScenePoint;

                    m_undoStack.push(new InsertRectTerrainCommand(selectedItems, m_ptrPixmapTerrainItem, fromScenePoint, toScenePoint, m_randomGenerator));
                }
            }
        }
    }
}

void FormTilesMapEditor::updateGrid()
{
    // Rimuovo le linee precedenti
    for (auto line : m_vLineItems)
        delete line;

    m_vLineItems.clear();

    // Creo la nuova griglia
    for (int x=0; x<=m_sceneMap.height(); x+=MAP_TILE_SIZE_PX)
        m_vLineItems.push_back(m_sceneMap.addLine(x, 0, x, m_sceneMap.width(), QPen(QColor::fromRgb(200, 200, 200), 1, Qt::DotLine)));

    for (int y=0; y<=m_sceneMap.width(); y+=MAP_TILE_SIZE_PX)
        m_vLineItems.push_back(m_sceneMap.addLine(0, y, m_sceneMap.height(), y, QPen(QColor::fromRgb(200, 200, 200), 1, Qt::DotLine)));

    for (auto line : m_vLineItems)
        line->setZValue(Z_VALUE_GRID);
}

void FormTilesMapEditor::loadResources()
{
    if (m_bResourcesLoaded)
        return;

    // -------------------------------------------------------------
    // USCITE
    // -------------------------------------------------------------
    {
        ui->tableWidgetExits->setRowCount(2);
        ui->tableWidgetExits->setColumnCount(4);

        QPixmap pixN = QPixmap(":/icons/icons/exit_north.svg");
        QPixmap pixS = QPixmap(":/icons/icons/exit_south.svg");
        QPixmap pixW = QPixmap(":/icons/icons/exit_weast.svg");
        QPixmap pixE = QPixmap(":/icons/icons/exit_east.svg");
        QPixmap pixU = QPixmap(":/icons/icons/exit_up.svg");
        QPixmap pixD = QPixmap(":/icons/icons/exit_down.svg");

        QTableWidgetItem* itemExitN = new QTableWidgetItem("");
        QTableWidgetItem* itemExitS = new QTableWidgetItem("");
        QTableWidgetItem* itemExitW = new QTableWidgetItem("");
        QTableWidgetItem* itemExitE = new QTableWidgetItem("");
        QTableWidgetItem* itemExitU = new QTableWidgetItem("");
        QTableWidgetItem* itemExitD = new QTableWidgetItem("");

        itemExitN->setData(Qt::DecorationRole, pixN.scaled(pixN.width() / 2, pixN.height() / 2));
        itemExitS->setData(Qt::DecorationRole, pixS.scaled(pixS.width() / 2, pixS.height() / 2));
        itemExitW->setData(Qt::DecorationRole, pixW.scaled(pixW.width() / 2, pixW.height() / 2));
        itemExitE->setData(Qt::DecorationRole, pixE.scaled(pixE.width() / 2, pixE.height() / 2));
        itemExitU->setData(Qt::DecorationRole, pixU.scaled(pixU.width() / 2, pixU.height() / 2));
        itemExitD->setData(Qt::DecorationRole, pixD.scaled(pixD.width() / 2, pixD.height() / 2));

        itemExitN->setSizeHint(QSize(pixN.width() / 2, pixN.height() / 2));
        itemExitS->setSizeHint(QSize(pixS.width() / 2, pixS.height() / 2));
        itemExitW->setSizeHint(QSize(pixW.width() / 2, pixW.height() / 2));
        itemExitE->setSizeHint(QSize(pixE.width() / 2, pixE.height() / 2));
        itemExitU->setSizeHint(QSize(pixU.width() / 2, pixU.height() / 2));
        itemExitD->setSizeHint(QSize(pixD.width() / 2, pixD.height() / 2));

        itemExitN->setData(USER_ROLE_PIXMAP, pixN);
        itemExitS->setData(USER_ROLE_PIXMAP, pixS);
        itemExitW->setData(USER_ROLE_PIXMAP, pixW);
        itemExitE->setData(USER_ROLE_PIXMAP, pixE);
        itemExitU->setData(USER_ROLE_PIXMAP, pixU);
        itemExitD->setData(USER_ROLE_PIXMAP, pixD);

        itemExitN->setData(USER_ROLE_FILE_NAME, "exitN");
        itemExitS->setData(USER_ROLE_FILE_NAME, "exitS");
        itemExitW->setData(USER_ROLE_FILE_NAME, "exitW");
        itemExitE->setData(USER_ROLE_FILE_NAME, "exitE");
        itemExitU->setData(USER_ROLE_FILE_NAME, "exitU");
        itemExitD->setData(USER_ROLE_FILE_NAME, "exitD");

        itemExitN->setData(USER_ROLE_FILE_PIXMAP_INDEX, 0);
        itemExitS->setData(USER_ROLE_FILE_PIXMAP_INDEX, 0);
        itemExitW->setData(USER_ROLE_FILE_PIXMAP_INDEX, 0);
        itemExitE->setData(USER_ROLE_FILE_PIXMAP_INDEX, 0);
        itemExitU->setData(USER_ROLE_FILE_PIXMAP_INDEX, 0);
        itemExitD->setData(USER_ROLE_FILE_PIXMAP_INDEX, 0);

        ui->tableWidgetExits->setItem(0, 0, itemExitN);
        ui->tableWidgetExits->setItem(0, 1, itemExitS);
        ui->tableWidgetExits->setItem(0, 2, itemExitW);
        ui->tableWidgetExits->setItem(0, 3, itemExitE);
        ui->tableWidgetExits->setItem(1, 0, itemExitU);
        ui->tableWidgetExits->setItem(1, 1, itemExitD);

        ui->tableWidgetExits->resizeRowsToContents();
        ui->tableWidgetExits->resizeColumnsToContents();
    }

    // -------------------------------------------------------------
    // ALBERI
    // -------------------------------------------------------------
    QStringList treeList = QDir("./editor/trees").entryList(QDir::Files);

    int tableRow = 0;

    ui->tableWidgetTrees->setColumnCount(4); // il 4 si riferisce alle versioni degli alberi per le 4 stagioni

    for (QString fileImage : treeList)
    {
        if (QFileInfo(fileImage).suffix() == "png" &&
                QFileInfo(fileImage).baseName().endsWith("_image"))
        {
            QString mapName = QFileInfo(fileImage).baseName().left(QFileInfo(fileImage).baseName().size() - QString("_image").size());

            for (QString fileRooms : treeList)
            {
                if (QFileInfo(fileRooms).suffix() == "png" &&
                        QFileInfo(fileRooms).baseName() == mapName + "_rooms")
                {
                    QImage image(QFileInfo(QDir("./editor/trees").path() + "/" + fileImage).filePath());
                    QImage rooms(QFileInfo(QDir("./editor/trees").path() + "/" + fileRooms).filePath());

                    if (image.size() == rooms.size() &&
                        image.width() % MAP_TILE_SIZE_PX == 0 &&
                        image.height() % MAP_TILE_SIZE_PX == 0)
                    {
                        QSize treeSize(image.width(), image.height() / 4); // il diviso 4 si riferisce alle versioni degli alberi per le 4 stagioni
                        QSize roomsSize(treeSize.width() / MAP_TILE_SIZE_PX, treeSize.height() / MAP_TILE_SIZE_PX);

                        QPixmap pixmapWinter = QPixmap::fromImage(image.copy(0, 0, treeSize.width(), treeSize.height()));
                        QPixmap pixmapSummer = QPixmap::fromImage(image.copy(0, treeSize.height(), treeSize.width(), treeSize.height()));
                        QPixmap pixmapSpring = QPixmap::fromImage(image.copy(0, treeSize.height() * 2, treeSize.width(), treeSize.height()));
                        QPixmap pixmapAutumn = QPixmap::fromImage(image.copy(0, treeSize.height() * 3, treeSize.width(), treeSize.height()));

                        FlagsMap roomsWinter(roomsSize);
                        FlagsMap roomsSummer(roomsSize);
                        FlagsMap roomsSpring(roomsSize);
                        FlagsMap roomsAutumn(roomsSize);

                        for (int x = 0; x < roomsSize.width(); ++x)
                        {
                            for (int y = 0; y < roomsSize.height(); ++y)
                            {
                                roomsWinter.setAt(x, y, qAlpha(rooms.pixel(x * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2, y * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2)) != 0);
                                roomsSummer.setAt(x, y, qAlpha(rooms.pixel(x * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2, y * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2 + treeSize.height())) != 0);
                                roomsSpring.setAt(x, y, qAlpha(rooms.pixel(x * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2, y * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2 + treeSize.height() * 2)) != 0);
                                roomsAutumn.setAt(x, y, qAlpha(rooms.pixel(x * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2, y * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2 + treeSize.height() * 3)) != 0);
                            }
                        }

                        QTableWidgetItem* itemWinter = new QTableWidgetItem("");
                        QTableWidgetItem* itemSummer = new QTableWidgetItem("");
                        QTableWidgetItem* itemSpring = new QTableWidgetItem("");
                        QTableWidgetItem* itemAutumn = new QTableWidgetItem("");

                        itemWinter->setData(Qt::DecorationRole, pixmapWinter.scaled(pixmapWinter.width() / 2, pixmapWinter.height() / 2));
                        itemSummer->setData(Qt::DecorationRole, pixmapSummer.scaled(pixmapSummer.width() / 2, pixmapSummer.height() / 2));
                        itemSpring->setData(Qt::DecorationRole, pixmapSpring.scaled(pixmapSpring.width() / 2, pixmapSpring.height() / 2));
                        itemAutumn->setData(Qt::DecorationRole, pixmapAutumn.scaled(pixmapAutumn.width() / 2, pixmapAutumn.height() / 2));

                        itemWinter->setSizeHint(QSize(pixmapWinter.width() / 2, pixmapWinter.height() / 2));
                        itemSummer->setSizeHint(QSize(pixmapSummer.width() / 2, pixmapSummer.height() / 2));
                        itemSpring->setSizeHint(QSize(pixmapSpring.width() / 2, pixmapSpring.height() / 2));
                        itemAutumn->setSizeHint(QSize(pixmapAutumn.width() / 2, pixmapAutumn.height() / 2));

                        itemWinter->setData(USER_ROLE_PIXMAP, pixmapWinter);
                        itemSummer->setData(USER_ROLE_PIXMAP, pixmapSummer);
                        itemSpring->setData(USER_ROLE_PIXMAP, pixmapSpring);
                        itemAutumn->setData(USER_ROLE_PIXMAP, pixmapAutumn);

                        itemWinter->setData(USER_ROLE_ROOMS, QVariant::fromValue(roomsWinter));
                        itemSummer->setData(USER_ROLE_ROOMS, QVariant::fromValue(roomsSummer));
                        itemSpring->setData(USER_ROLE_ROOMS, QVariant::fromValue(roomsSpring));
                        itemAutumn->setData(USER_ROLE_ROOMS, QVariant::fromValue(roomsAutumn));

                        itemWinter->setData(USER_ROLE_FILE_NAME, fileImage);
                        itemSummer->setData(USER_ROLE_FILE_NAME, fileImage);
                        itemSpring->setData(USER_ROLE_FILE_NAME, fileImage);
                        itemAutumn->setData(USER_ROLE_FILE_NAME, fileImage);

                        itemWinter->setData(USER_ROLE_FILE_PIXMAP_INDEX, 0);
                        itemSummer->setData(USER_ROLE_FILE_PIXMAP_INDEX, 1);
                        itemSpring->setData(USER_ROLE_FILE_PIXMAP_INDEX, 2);
                        itemAutumn->setData(USER_ROLE_FILE_PIXMAP_INDEX, 3);

                        ui->tableWidgetTrees->setRowCount(tableRow+1);
                        ui->tableWidgetTrees->setItem(tableRow, 0, itemWinter);
                        ui->tableWidgetTrees->setItem(tableRow, 1, itemSummer);
                        ui->tableWidgetTrees->setItem(tableRow, 2, itemSpring);
                        ui->tableWidgetTrees->setItem(tableRow, 3, itemAutumn);

                        tableRow++;
                    }
                }
            }
        }
    }

    ui->tableWidgetTrees->resizeRowsToContents();
    ui->tableWidgetTrees->resizeColumnsToContents();

    // -------------------------------------------------------------
    // EDIFICI
    // -------------------------------------------------------------
    QStringList buildingsList = QDir("./editor/buildings").entryList(QDir::Files);

    tableRow = 0;

    ui->tableWidgetBuildings->setColumnCount(1); // il 4 si riferisce alle versioni degli alberi per le 4 stagioni

    for (QString fileImage : buildingsList)
    {
        if (QFileInfo(fileImage).suffix() == "png" &&
                QFileInfo(fileImage).baseName().endsWith("_image"))
        {
            QString mapName = QFileInfo(fileImage).baseName().left(QFileInfo(fileImage).baseName().size() - QString("_image").size());

            for (QString fileRooms : buildingsList)
            {
                if (QFileInfo(fileRooms).suffix() == "png" &&
                        QFileInfo(fileRooms).baseName() == mapName + "_rooms")
                {
                    QImage image(QFileInfo(QDir("./editor/buildings").path() + "/" + fileImage).filePath());
                    QImage rooms(QFileInfo(QDir("./editor/buildings").path() + "/" + fileRooms).filePath());

                    if (image.size() == rooms.size() &&
                        image.width() % MAP_TILE_SIZE_PX == 0 &&
                        image.height() % MAP_TILE_SIZE_PX == 0)
                    {
                        QSize buildingSize(image.width(), image.height());
                        QSize roomsSize(buildingSize.width() / MAP_TILE_SIZE_PX, buildingSize.height() / MAP_TILE_SIZE_PX);

                        QPixmap pixmap = QPixmap::fromImage(image.copy(0, 0, buildingSize.width(), buildingSize.height()));

                        FlagsMap roomsFlags(roomsSize);

                        for (int x = 0; x < roomsSize.width(); ++x)
                        {
                            for (int y = 0; y < roomsSize.height(); ++y)
                            {
                                roomsFlags.setAt(x, y, qAlpha(rooms.pixel(x * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2, y * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2)) != 0);
                            }
                        }

                        QTableWidgetItem* item = new QTableWidgetItem("");
                        item->setData(Qt::DecorationRole, pixmap.scaled(pixmap.width() / 2, pixmap.height() / 2));
                        item->setSizeHint(QSize(pixmap.width() / 2, pixmap.height() / 2));
                        item->setData(USER_ROLE_PIXMAP, pixmap);
                        item->setData(USER_ROLE_ROOMS, QVariant::fromValue(roomsFlags));
                        item->setData(USER_ROLE_FILE_NAME, fileImage);
                        item->setData(USER_ROLE_FILE_PIXMAP_INDEX, 0);

                        ui->tableWidgetBuildings->setRowCount(tableRow+1);
                        ui->tableWidgetBuildings->setItem(tableRow, 0, item);

                        tableRow++;
                    }
                }
            }
        }
    }

    ui->tableWidgetBuildings->resizeRowsToContents();
    ui->tableWidgetBuildings->resizeColumnsToContents();

    // -------------------------------------------------------------
    // TERRENI
    // -------------------------------------------------------------
    QStringList terrainList = QDir("./editor/terrains").entryList(QDir::Files);

    tableRow = 0;

    for (QString fileImage : terrainList)
    {
        if (QFileInfo(fileImage).suffix() == "png" &&
                QFileInfo(fileImage).baseName().endsWith("_image"))
        {
            QString terrainName = QFileInfo(fileImage).baseName().left(QFileInfo(fileImage).baseName().size() - QString("_image").size());

            for (QString fileRooms : terrainList)
            {
                if (QFileInfo(fileRooms).suffix() == "png" &&
                    QFileInfo(fileRooms).baseName() == terrainName + "_rooms")
                {
                    QImage image(QFileInfo(QDir("./editor/terrains").path() + "/" + fileImage).filePath());
                    QImage rooms(QFileInfo(QDir("./editor/terrains").path() + "/" + fileRooms).filePath());

                    if (image.size() == rooms.size() &&
                        image.width() % MAP_TILE_SIZE_PX == 0 &&
                        image.height() % MAP_TILE_SIZE_PX == 0)
                    {
                        QSize terrainSize(image.width() / MAP_TILE_SIZE_PX, image.height() / MAP_TILE_SIZE_PX);
                        QSize roomsSize(image.width() / MAP_TILE_SIZE_PX, image.height() / MAP_TILE_SIZE_PX);

                        for (int y = 0; y < terrainSize.height(); ++y)
                        {
                            for (int x = 0; x < terrainSize.width(); ++x)
                            {
                                QTableWidgetItem* item = new QTableWidgetItem();

                                QPixmap pixmap = QPixmap::fromImage(image.copy(x * MAP_TILE_SIZE_PX, y * MAP_TILE_SIZE_PX, MAP_TILE_SIZE_PX, MAP_TILE_SIZE_PX));
                                FlagsMap roomsMap(QSize(1, 1));

                                roomsMap.setAt(0, 0, qAlpha(rooms.pixel(x * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2, y * MAP_TILE_SIZE_PX + MAP_TILE_SIZE_PX / 2)) != 0);

                                item->setSizeHint(QSize(pixmap.width(), pixmap.height()));
                                item->setData(USER_ROLE_PIXMAP, pixmap);
                                item->setData(USER_ROLE_ROOMS, QVariant::fromValue(roomsMap));
                                item->setData(USER_ROLE_FILE_NAME, fileImage);
                                item->setData(USER_ROLE_FILE_PIXMAP_INDEX, 0);

                                if (ui->tableWidgetTerrains->rowCount() != tableRow + 1)
                                    ui->tableWidgetTerrains->setRowCount(tableRow + 1);

                                if (ui->tableWidgetTerrains->columnCount() < x + 1)
                                    ui->tableWidgetTerrains->setColumnCount(x + 1);

                                ui->tableWidgetTerrains->setItem(tableRow, x, item);
                            }

                            tableRow++;
                        }
                    }
                }
            }
        }
    }

    ui->tableWidgetTerrains->resizeRowsToContents();
    ui->tableWidgetTerrains->resizeColumnsToContents();

    m_bResourcesLoaded = true;
}

void FormTilesMapEditor::updateActionsStatus(QObject* obj)
{
    if (obj == m_ptrRectangularSelection && m_ptrRectangularSelection->isChecked())
    {
        clearSelection(m_ptrRectangularSelection);

        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        ui->graphicsView->setCursor(QPixmap(":/icons/icons/rectangular_selection.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    else if (obj == m_ptrDrawTerrainRect && m_ptrDrawTerrainRect->isChecked())
    {
        clearSelection(m_ptrDrawTerrainRect);

        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        ui->graphicsView->setCursor(QPixmap(":/icons/icons/rectangular_draw.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    else if (obj == m_ptrRubber && m_ptrRubber->isChecked())
    {
        clearSelection(m_ptrRubber);

        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        ui->graphicsView->setCursor(QPixmap(":/icons/icons/rubber_cursor.svg").scaled(24, 24, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        if (m_currentSelection.type != CurrentSelection::Type::NONE)
            ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        else
            ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

        ui->graphicsView->setCursor(Qt::ArrowCursor);
    }
}

void FormTilesMapEditor::clearSelection(QObject* excluded)
{
    if (excluded != ui->tableWidgetTrees)
        ui->tableWidgetTrees->clearSelection();

    if (excluded != ui->tableWidgetBuildings)
        ui->tableWidgetBuildings->clearSelection();

    if (excluded != ui->tableWidgetExits)
        ui->tableWidgetExits->clearSelection();

    if (excluded != ui->tableWidgetTerrains && excluded != m_ptrDrawTerrainRect)
        ui->tableWidgetTerrains->clearSelection();

    if (excluded != m_ptrRectangularSelection)
        m_ptrRectangularSelection->setChecked(false);

    if (excluded != m_ptrRubber)
        m_ptrRubber->setChecked(false);

    if (excluded != ui->tableWidgetTerrains && excluded != m_ptrDrawTerrainRect)
        m_ptrDrawTerrainRect->setChecked(false);

    if (!m_ptrDrawTerrainRect->isChecked())
    {
        m_currentSelection = CurrentSelection();

        if (m_ptrCurrentSelectionItem)
        {
            delete m_ptrCurrentSelectionItem;
            m_ptrCurrentSelectionItem = Q_NULLPTR;
        }

        ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    m_sceneMap.clearSelection();
}

MyGraphicsItem *FormTilesMapEditor::createExit(QPixmap pixmap, QString fileName, int filePixmapIndex, const EditExitDialog::ExitData& exitData)
{
    MyGraphicsItem* newItem = new MyGraphicsItem(pixmap, &m_undoStack, this);
    newItem->setData(USER_ROLE_TYPE, QVariant::fromValue(static_cast<int>(CurrentSelection::Type::EXIT)));
    newItem->setData(USER_ROLE_FILE_NAME, fileName);
    newItem->setData(USER_ROLE_FILE_PIXMAP_INDEX, filePixmapIndex);
    newItem->setFlag(QGraphicsItem::ItemIsSelectable);
    newItem->setFlag(QGraphicsItem::ItemIsMovable);
    newItem->setZValue(Z_VALUE_EXITS);

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(20);
    effect->setOffset(0);
    effect->setColor(QColor(0, 0, 0, 255));

    newItem->setGraphicsEffect(effect);

    newItem->setData(USER_ROLE_EXIT_DATA, QVariant::fromValue(exitData));

    return newItem;
}

MyGraphicsItem *FormTilesMapEditor::createTree(QPixmap pixmap, const FlagsMap &map, QString fileName, int filePixmapIndex)
{
    MyGraphicsItem* newItem = new MyGraphicsItem(pixmap, &m_undoStack, this);
    newItem->setData(USER_ROLE_ROOMS, QVariant::fromValue(map));
    newItem->setData(USER_ROLE_TYPE, QVariant::fromValue(static_cast<int>(CurrentSelection::Type::TREE)));
    newItem->setData(USER_ROLE_FILE_NAME, fileName);
    newItem->setData(USER_ROLE_FILE_PIXMAP_INDEX, filePixmapIndex);
    newItem->setFlag(QGraphicsItem::ItemIsSelectable);
    newItem->setFlag(QGraphicsItem::ItemIsMovable);
    newItem->setZValue(Z_VALUE_TREE);

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(20);
    effect->setOffset(0);
    effect->setColor(QColor(0, 0, 0, 255));

    newItem->setGraphicsEffect(effect);

    return newItem;
}

MyGraphicsItem *FormTilesMapEditor::createBuilding(QPixmap pixmap, const FlagsMap &map, QString fileName, int filePixmapIndex)
{
    MyGraphicsItem* newItem = new MyGraphicsItem(pixmap, &m_undoStack, this);
    newItem->setData(USER_ROLE_ROOMS, QVariant::fromValue(map));
    newItem->setData(USER_ROLE_TYPE, QVariant::fromValue(static_cast<int>(CurrentSelection::Type::BUILDING)));
    newItem->setData(USER_ROLE_FILE_NAME, fileName);
    newItem->setData(USER_ROLE_FILE_PIXMAP_INDEX, filePixmapIndex);
    newItem->setFlag(QGraphicsItem::ItemIsSelectable);
    newItem->setFlag(QGraphicsItem::ItemIsMovable);
    newItem->setZValue(Z_VALUE_BUILDING);

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(20);
    effect->setOffset(0);
    effect->setColor(QColor(0, 0, 0, 255));

    newItem->setGraphicsEffect(effect);

    return newItem;
}

QVector<FormTilesMapEditor::ObjectData> FormTilesMapEditor::objectDataListFromString(QString str)
{
    QVector<FormTilesMapEditor::ObjectData> list;

    {
        QStringList lines = str.split("\n");

        for (auto line : lines)
        {
            if (line.simplified().isEmpty())
                continue;

            QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8());
            QJsonObject obj = doc.object();

            FormTilesMapEditor::ObjectData data;

            data.type = CurrentSelection::typeFromString(obj["type"].toString());
            data.filename = obj["filename"].toString();
            data.index = obj["index"].toString().toInt();
            data.x = obj["x"].toString().toInt();
            data.y = obj["y"].toString().toInt();

            if (data.type == CurrentSelection::Type::EXIT)
            {
                EditExitDialog::ExitData exitData;
                exitData.deserialize(obj["specificData"].toObject());
                data.specificData = QVariant::fromValue(exitData);
            }

            list.push_front(data);
        }
    }

    return list;
}

QString FormTilesMapEditor::objectDataListToString()
{
    QString objects;

    auto items = m_sceneMap.items();

    for (int i = 0; i < items.size(); ++i)
    {
        if (items[i]->type() == USER_TYPE_MY_GRAPHICS_ITEM)
        {
            QJsonObject obj;

            MyGraphicsItem* item = static_cast<MyGraphicsItem*>(items[i]);

            obj["type"] = CurrentSelection::typeToString(static_cast<CurrentSelection::Type>(item->data(USER_ROLE_TYPE).toInt()));
            obj["filename"] = item->data(USER_ROLE_FILE_NAME).toString();
            obj["index"] = QString::number(item->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt());
            obj["x"] = QString::number(qRound(item->x() / MAP_TILE_SIZE_PX));
            obj["y"] = QString::number(qRound(item->y() / MAP_TILE_SIZE_PX));

            if (static_cast<CurrentSelection::Type>(item->data(USER_ROLE_TYPE).toInt()) == CurrentSelection::Type::EXIT)
            {
                QJsonObject specificObj;
                item->data(USER_ROLE_EXIT_DATA).value<EditExitDialog::ExitData>().serialize(specificObj);
                obj["specificData"] = specificObj;
            }

            QJsonDocument doc = QJsonDocument(obj);
            objects += doc.toJson(QJsonDocument::Compact) + "\n";
        }
    }

    return objects;
}

void FormTilesMapEditor::on_tableWidgetExits_itemSelectionChanged()
{
    if (!ui->tableWidgetExits->selectedItems().isEmpty())
    {
        QTableWidgetItem* itemSelected = ui->tableWidgetExits->selectedItems()[0];

        clearSelection(ui->tableWidgetExits);

        CurrentSelection selection;
        selection.type = CurrentSelection::Type::EXIT;
        selection.image = itemSelected->data(USER_ROLE_PIXMAP).value<QPixmap>();
        selection.rooms = FlagsMap();
        m_ptrCurrentSelectionItem = new MyGraphicsItem(selection.image, &m_undoStack, this);
        m_ptrCurrentSelectionItem->setData(USER_ROLE_TYPE, itemSelected->data(USER_ROLE_TYPE));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_FILE_NAME, itemSelected->data(USER_ROLE_FILE_NAME));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_FILE_PIXMAP_INDEX, itemSelected->data(USER_ROLE_FILE_PIXMAP_INDEX));

        m_sceneMap.addItem(m_ptrCurrentSelectionItem);

        m_ptrCurrentSelectionItem->setVisible(false);
        m_ptrCurrentSelectionItem->setZValue(Z_VALUE_CURRENT_SELECTION);

        m_currentSelection = selection;

        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    }
}

void FormTilesMapEditor::on_tableWidgetTrees_itemSelectionChanged()
{
    if (!ui->tableWidgetTrees->selectedItems().isEmpty())
    {
        QTableWidgetItem* itemSelected = ui->tableWidgetTrees->selectedItems()[0];

        clearSelection(ui->tableWidgetTrees);

        CurrentSelection selection;
        selection.type = CurrentSelection::Type::TREE;
        selection.image = itemSelected->data(USER_ROLE_PIXMAP).value<QPixmap>();
        selection.rooms = itemSelected->data(USER_ROLE_ROOMS).value<FlagsMap>();
        m_ptrCurrentSelectionItem = new MyGraphicsItem(selection.image, &m_undoStack, this);
        m_ptrCurrentSelectionItem->setData(USER_ROLE_ROOMS, itemSelected->data(USER_ROLE_ROOMS));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_TYPE, itemSelected->data(USER_ROLE_TYPE));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_FILE_NAME, itemSelected->data(USER_ROLE_FILE_NAME));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_FILE_PIXMAP_INDEX, itemSelected->data(USER_ROLE_FILE_PIXMAP_INDEX));

        m_sceneMap.addItem(m_ptrCurrentSelectionItem);

        m_ptrCurrentSelectionItem->setVisible(false);
        m_ptrCurrentSelectionItem->setZValue(Z_VALUE_CURRENT_SELECTION);

        m_currentSelection = selection;

        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    }
}

void FormTilesMapEditor::on_tableWidgetBuildings_itemSelectionChanged()
{
    if (!ui->tableWidgetBuildings->selectedItems().isEmpty())
    {
        QTableWidgetItem* itemSelected = ui->tableWidgetBuildings->selectedItems()[0];

        clearSelection(ui->tableWidgetBuildings);

        CurrentSelection selection;
        selection.type = CurrentSelection::Type::BUILDING;
        selection.image = itemSelected->data(USER_ROLE_PIXMAP).value<QPixmap>();
        selection.rooms = itemSelected->data(USER_ROLE_ROOMS).value<FlagsMap>();
        m_ptrCurrentSelectionItem = new MyGraphicsItem(selection.image, &m_undoStack, this);
        m_ptrCurrentSelectionItem->setData(USER_ROLE_ROOMS, itemSelected->data(USER_ROLE_ROOMS));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_TYPE, itemSelected->data(USER_ROLE_TYPE));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_FILE_NAME, itemSelected->data(USER_ROLE_FILE_NAME));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_FILE_PIXMAP_INDEX, itemSelected->data(USER_ROLE_FILE_PIXMAP_INDEX));

        m_sceneMap.addItem(m_ptrCurrentSelectionItem);

        m_ptrCurrentSelectionItem->setVisible(false);
        m_ptrCurrentSelectionItem->setZValue(Z_VALUE_CURRENT_SELECTION);

        m_currentSelection = selection;

        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    }
}

void FormTilesMapEditor::on_tableWidgetTerrains_itemSelectionChanged()
{
    if (!ui->tableWidgetTerrains->selectedItems().isEmpty())
    {
        QTableWidgetItem* itemSelected = ui->tableWidgetTerrains->selectedItems()[0];

        clearSelection(ui->tableWidgetTerrains);

        CurrentSelection selection;
        selection.type = CurrentSelection::Type::TERRAIN;
        selection.image = itemSelected->data(USER_ROLE_PIXMAP).value<QPixmap>();
        m_ptrCurrentSelectionItem = new MyGraphicsItem(selection.image, &m_undoStack, this);
        m_ptrCurrentSelectionItem->setData(USER_ROLE_ROOMS, itemSelected->data(USER_ROLE_ROOMS));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_TYPE, itemSelected->data(USER_ROLE_TYPE));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_FILE_NAME, itemSelected->data(USER_ROLE_FILE_NAME));
        m_ptrCurrentSelectionItem->setData(USER_ROLE_FILE_PIXMAP_INDEX, itemSelected->data(USER_ROLE_FILE_PIXMAP_INDEX));

        m_sceneMap.addItem(m_ptrCurrentSelectionItem);

        m_ptrCurrentSelectionItem->setVisible(false);
        m_ptrCurrentSelectionItem->setZValue(Z_VALUE_CURRENT_SELECTION);

        m_currentSelection = selection;

        if (!m_ptrDrawTerrainRect->isChecked())
            ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    }
}

void FormTilesMapEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        clearSelection();
    }
    else if (event->key() == Qt::Key_Delete)
    {
        auto selectedItems = m_sceneMap.selectedItems();

        if (!selectedItems.isEmpty())
            m_undoStack.push(new RemoveItemsCommand(selectedItems, &m_sceneMap, &m_sceneMapRemovedItems));
    }
}

bool FormTilesMapEditor::eventFilter(QObject *object, QEvent *event)
{
    if (object == &m_sceneMap)
    {
        if (event->type() == QEvent::Leave)
        {
            if (m_ptrCurrentSelectionItem)
                m_ptrCurrentSelectionItem->setVisible(false);
        }
        else if (event->type() == QEvent::GraphicsSceneMouseMove)
        {
            QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

            if (m_ptrCurrentSelectionItem && QApplication::mouseButtons() == Qt::NoButton)
            {
                if (!m_ptrDrawTerrainRect->isChecked())
                {
                    m_ptrCurrentSelectionItem->setVisible(true);
                    m_ptrCurrentSelectionItem->setPos(mouseEvent->scenePos());
                }
            }
            else if (QApplication::mouseButtons() == Qt::LeftButton &&
                     !m_ptrDrawTerrainRect->isChecked())
            {
                if (m_ptrCurrentSelectionItem && !m_ptrDrawTerrainRect->isChecked())
                    m_ptrCurrentSelectionItem->setPos(mouseEvent->scenePos());

                if (m_ptrRubber->isChecked())
                {
                    QPointF pos = mouseEvent->scenePos();
                    pos.setX(qRound(pos.x()/MAP_TILE_SIZE_PX - 0.5) * MAP_TILE_SIZE_PX);
                    pos.setY(qRound(pos.y()/MAP_TILE_SIZE_PX - 0.5) * MAP_TILE_SIZE_PX);

                    if (pos != m_lastTerrainPos)
                    {
                        m_undoStack.push(new DeleteTerrainCommand(m_ptrPixmapTerrainItem, QPoint(qRound(pos.x()), qRound(pos.y()))));
                        m_lastTerrainPos = pos;
                    }
                }
                else if (m_ptrCurrentSelectionItem && m_currentSelection.type == CurrentSelection::Type::TERRAIN && m_ptrCurrentSelectionItem->pos() != m_lastTerrainPos)
                {
                    QPointF pos = m_ptrCurrentSelectionItem->pos();

                    auto selectedItems = ui->tableWidgetTerrains->selectedItems();

                    if (!selectedItems.isEmpty())
                    {
                        auto item = selectedItems[m_randomGenerator.bounded(0, selectedItems.size())];

                        m_undoStack.push(new InsertTerrainCommand(item, m_ptrPixmapTerrainItem, QPoint(qRound(pos.x()), qRound(pos.y()))));

                        m_lastTerrainPos = pos;
                    }
                }
            }
        }
        else if (event->type() == QEvent::GraphicsSceneMouseRelease)
        {
            QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

            m_lastTerrainPos = QPointF(-1, -1);

            if (mouseEvent->button() == Qt::MouseButton::LeftButton &&
                    m_ptrCurrentSelectionItem)
            {
                if (m_currentSelection.type == CurrentSelection::Type::EXIT)
                {
                    MyGraphicsItem* newItem = createExit(m_currentSelection.image,
                                                         m_ptrCurrentSelectionItem->data(USER_ROLE_FILE_NAME).toString(),
                                                         m_ptrCurrentSelectionItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt(),
                                                         EditExitDialog::ExitData());

                    m_undoStack.push(new InsertItemCommand(newItem, &m_sceneMap, mouseEvent->scenePos()));
                    ui->labelInfo->setText(QString("%1").arg(m_sceneMap.items().count()));
                }
                else if (m_currentSelection.type == CurrentSelection::Type::TREE)
                {
                    MyGraphicsItem* newItem = createTree(m_currentSelection.image, m_currentSelection.rooms,
                                                         m_ptrCurrentSelectionItem->data(USER_ROLE_FILE_NAME).toString(),
                                                         m_ptrCurrentSelectionItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt());

                    m_undoStack.push(new InsertItemCommand(newItem, &m_sceneMap, mouseEvent->scenePos()));
                    ui->labelInfo->setText(QString("%1").arg(m_sceneMap.items().count()));
                }
                else if (m_currentSelection.type == CurrentSelection::Type::BUILDING)
                {
                    MyGraphicsItem* newItem = createBuilding(m_currentSelection.image, m_currentSelection.rooms,
                                                         m_ptrCurrentSelectionItem->data(USER_ROLE_FILE_NAME).toString(),
                                                         m_ptrCurrentSelectionItem->data(USER_ROLE_FILE_PIXMAP_INDEX).toInt());

                    m_undoStack.push(new InsertItemCommand(newItem, &m_sceneMap, mouseEvent->scenePos()));
                    ui->labelInfo->setText(QString("%1").arg(m_sceneMap.items().count()));
                }
            }
        }
        else if (event->type() == QEvent::GraphicsSceneMousePress)
        {
            QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);

            if (mouseEvent->button() == Qt::MouseButton::LeftButton &&
                    !m_ptrDrawTerrainRect->isChecked())
            {
                if (m_ptrRubber->isChecked())
                {
                    QPointF pos = mouseEvent->scenePos();
                    pos.setX(qRound(pos.x()/MAP_TILE_SIZE_PX - 0.5) * MAP_TILE_SIZE_PX);
                    pos.setY(qRound(pos.y()/MAP_TILE_SIZE_PX - 0.5) * MAP_TILE_SIZE_PX);

                    m_undoStack.push(new DeleteTerrainCommand(m_ptrPixmapTerrainItem, QPoint(qRound(pos.x()), qRound(pos.y()))));

                    m_lastTerrainPos = pos;
                }
                else if (m_ptrCurrentSelectionItem && m_currentSelection.type == CurrentSelection::Type::TERRAIN)
                {
                    QPointF pos = m_ptrCurrentSelectionItem->pos();

                    auto selectedItems = ui->tableWidgetTerrains->selectedItems();

                    if (!selectedItems.isEmpty())
                    {
                        auto item = selectedItems[m_randomGenerator.bounded(0, selectedItems.size())];

                        m_undoStack.push(new InsertTerrainCommand(item, m_ptrPixmapTerrainItem, QPoint(qRound(pos.x()), qRound(pos.y()))));

                        m_lastTerrainPos = pos;
                    }
                }
            }
        }
    }

    return false;
}

void FormTilesMapEditor::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    hide();

    emit editComplete();
}

MyGraphicsItem::MyGraphicsItem(const QPixmap &pixmap, QUndoStack *undoStack, QWidget *parentWnd, QGraphicsItem *parent) :
    QGraphicsPixmapItem(pixmap, parent),
    m_ptrUndoStack(undoStack),
    m_ptrParentWnd(parentWnd)
{
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
}

QVariant MyGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        QPointF pos = value.toPointF();
        pos.setX(pos.x());
        pos.setY(pos.y());

        pos.setX(floor(pos.x() / MAP_TILE_SIZE_PX) * MAP_TILE_SIZE_PX);
        pos.setY(floor(pos.y() / MAP_TILE_SIZE_PX) * MAP_TILE_SIZE_PX);

        if (pos.x() < 0.0)
            pos.setX(0.0);

        if (pos.x() > scene()->width() - pixmap().width())
            pos.setX(scene()->width() - pixmap().width());

        if (pos.y() < 0.0)
            pos.setY(0.0);

        if (pos.y() > scene()->height() - pixmap().height())
            pos.setY(scene()->height() - pixmap().height());

        return pos;
    }

    return QGraphicsItem::itemChange(change, value);
}

void MyGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    m_oldPos = scenePos();

    QGraphicsPixmapItem::mousePressEvent(event);
}

void MyGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_oldPos != scenePos())
        m_ptrUndoStack->push(new MoveItemCommand(this, scenePos(), m_oldPos));

    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void MyGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    FormTilesMapEditor::CurrentSelection::Type type = static_cast<FormTilesMapEditor::CurrentSelection::Type>(data(USER_ROLE_TYPE).toInt());

    if (type == FormTilesMapEditor::CurrentSelection::Type::EXIT)
    {
        EditExitDialog dlg(m_ptrParentWnd);
        dlg.setData(data(USER_ROLE_EXIT_DATA).value<EditExitDialog::ExitData>());

        if (dlg.exec() == QDialog::Accepted)
            setData(USER_ROLE_EXIT_DATA, QVariant::fromValue(dlg.data()));
    }
}

int MyGraphicsItem::type() const
{
    return USER_TYPE_MY_GRAPHICS_ITEM;
}

void ImageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QPixmap pixmap = index.data(USER_ROLE_PIXMAP).value<QPixmap>();

    painter->drawPixmap(option.rect, pixmap, pixmap.rect());

    if (option.state & QStyle::State_Selected)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 0, 200, 60));
        painter->drawRect(option.rect);
    }
}

QSize ImageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    QPixmap pixmap = index.data(USER_ROLE_PIXMAP).value<QPixmap>();

    return pixmap.size() / 2;
}


MyGraphicsImageItem::MyGraphicsImageItem(const QPixmap &pixmap, const FlagsMap &rooms, QGraphicsItem *parent)
    : QGraphicsItem(parent),
      m_pixmap(pixmap),
      m_vRooms(rooms)
{

}

QRectF MyGraphicsImageItem::boundingRect() const
{
    return m_pixmap.rect();
}

void MyGraphicsImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->drawPixmap(m_pixmap.rect(), m_pixmap, m_pixmap.rect());
}

QPixmap& MyGraphicsImageItem::pixmap()
{
    return m_pixmap;
}

FlagsMap &MyGraphicsImageItem::rooms()
{
    return m_vRooms;
}

FlagsMap MyGraphicsImageItem::rooms() const
{
    return m_vRooms;
}

InsertRectTerrainCommand::InsertRectTerrainCommand(QList<QTableWidgetItem *> selectedItems, MyGraphicsImageItem *item, QPointF from, QPointF to, QRandomGenerator &randomGenerator)
    : m_fromScenePoint(from),
      m_toScenePoint(to),
      m_selectedItems(selectedItems),
      m_randomGenerator(randomGenerator),
      m_item(item)
{ }

void InsertRectTerrainCommand::redo()
{
    QPixmap& pixmap = m_item->pixmap();
    FlagsMap& rooms = m_item->rooms();

    QRect rect(QPoint(qMax(qRound(m_fromScenePoint.x()/MAP_TILE_SIZE_PX + 0.5), 0) * MAP_TILE_SIZE_PX,
                      qMax(qRound(m_fromScenePoint.y()/MAP_TILE_SIZE_PX + 0.5), 0) * MAP_TILE_SIZE_PX),
               QPoint(qMin((qRound(m_toScenePoint.x()/MAP_TILE_SIZE_PX - 0.5)) * MAP_TILE_SIZE_PX, qRound(m_item->scene()->width())) ,
                      qMin((qRound(m_toScenePoint.y()/MAP_TILE_SIZE_PX - 0.5)) * MAP_TILE_SIZE_PX, qRound(m_item->scene()->height()))));

    m_oldPixmap = pixmap.copy(rect);

    m_vOldIsRooms = m_item->rooms();

    {
        QPainter painter(&pixmap);

        for (int x = qMax(qRound(m_fromScenePoint.x()/MAP_TILE_SIZE_PX + 0.5), 0);
             x < qMin(qRound(m_toScenePoint.x()/MAP_TILE_SIZE_PX - 0.5), qRound(m_item->scene()->width()/MAP_TILE_SIZE_PX));
             x++)
        {
            for (int y = qMax(qRound(m_fromScenePoint.y()/MAP_TILE_SIZE_PX + 0.5), 0);
                 y < qMin(qRound(m_toScenePoint.y()/MAP_TILE_SIZE_PX - 0.5), qRound(m_item->scene()->height()/MAP_TILE_SIZE_PX));
                 y++)
            {
                auto item = m_selectedItems[m_randomGenerator.bounded(0, m_selectedItems.size())];

                QPoint pos(x * MAP_TILE_SIZE_PX, y * MAP_TILE_SIZE_PX);

                painter.drawPixmap(pos, item->data(USER_ROLE_PIXMAP).value<QPixmap>());

                rooms.setAt(x, y, item->data(USER_ROLE_ROOMS).value<FlagsMap>().at(0, 0));
            }
        }

        painter.end();
    }

    m_item->update();
}

void InsertRectTerrainCommand::undo()
{
    QPixmap& pixmap = m_item->pixmap();
    FlagsMap& rooms = m_item->rooms();

    {
        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawPixmap(qMax(qRound(m_fromScenePoint.x()/MAP_TILE_SIZE_PX + 0.5), 0) * MAP_TILE_SIZE_PX,
                           qMax(qRound(m_fromScenePoint.y()/MAP_TILE_SIZE_PX + 0.5), 0) * MAP_TILE_SIZE_PX,
                           m_oldPixmap);
        painter.end();
    }

    m_item->update();

    rooms = m_vOldIsRooms;
}

InsertTerrainCommand::InsertTerrainCommand(QTableWidgetItem *selectedItem, MyGraphicsImageItem *item, QPoint pos)
    : m_selectedItem(selectedItem),
      m_item(item),
      m_pos(pos)
{

}

void InsertTerrainCommand::redo()
{
    QPixmap& pixmap = m_item->pixmap();
    FlagsMap& rooms = m_item->rooms();

    QPixmap newPixmap = m_selectedItem->data(USER_ROLE_PIXMAP).value<QPixmap>();
    bool isRoom = m_selectedItem->data(USER_ROLE_ROOMS).value<FlagsMap>().at(0, 0);

    m_oldPixmap = pixmap.copy(m_pos.x(), m_pos.y(), newPixmap.width(), newPixmap.height());
    m_bOldIsRoom = rooms.at(m_pos.x()/MAP_TILE_SIZE_PX, m_pos.y()/MAP_TILE_SIZE_PX);

    {
        QPainter painter(&pixmap);
        painter.drawPixmap(m_pos.x(), m_pos.y(), newPixmap);
        painter.end();
    }

    rooms.setAt(m_pos.x()/MAP_TILE_SIZE_PX, m_pos.y()/MAP_TILE_SIZE_PX, isRoom);

    m_item->update();
}

void InsertTerrainCommand::undo()
{
    QPixmap& pixmap = m_item->pixmap();
    FlagsMap& rooms = m_item->rooms();

    {
        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawPixmap(m_pos.x(), m_pos.y(), m_oldPixmap);
        painter.end();
    }

    rooms.setAt(m_pos.x()/MAP_TILE_SIZE_PX, m_pos.y()/MAP_TILE_SIZE_PX, m_bOldIsRoom);

    m_item->update();
}

DeleteTerrainCommand::DeleteTerrainCommand(MyGraphicsImageItem *item, QPoint pos)
    : m_item(item),
      m_pos(pos)
{

}

void DeleteTerrainCommand::redo()
{
    QPixmap& pixmap = m_item->pixmap();
    FlagsMap& rooms = m_item->rooms();

    m_oldPixmap = pixmap.copy(m_pos.x(), m_pos.y(), MAP_TILE_SIZE_PX, MAP_TILE_SIZE_PX);
    m_bOldIsRoom = rooms.at(m_pos.x()/MAP_TILE_SIZE_PX, m_pos.y()/MAP_TILE_SIZE_PX);

    {
        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setBrush(QColor(0, 0, 0, 0));
        painter.setPen(Qt::NoPen);
        painter.drawRect(m_pos.x(), m_pos.y(), MAP_TILE_SIZE_PX, MAP_TILE_SIZE_PX);
        painter.end();
    }

    rooms.setAt(m_pos.x()/MAP_TILE_SIZE_PX, m_pos.y()/MAP_TILE_SIZE_PX, false);

    m_item->update();
}

void DeleteTerrainCommand::undo()
{
    QPixmap& pixmap = m_item->pixmap();
    FlagsMap& rooms = m_item->rooms();

    {
        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawPixmap(m_pos.x(), m_pos.y(), m_oldPixmap);
        painter.end();
    }

    rooms.setAt(m_pos.x()/MAP_TILE_SIZE_PX, m_pos.y()/MAP_TILE_SIZE_PX, m_bOldIsRoom);

    m_item->update();
}

RemoveItemsCommand::RemoveItemsCommand(QList<QGraphicsItem *> items, QGraphicsScene *scene, QGraphicsScene *sceneRemovedItems)
    : m_items(items),
      m_scene(scene),
      m_sceneRemovedItems(sceneRemovedItems)
{

}

void RemoveItemsCommand::redo()
{
    for (auto item : m_items)
        m_scene->removeItem(item);

    for (auto item : m_items)
        m_sceneRemovedItems->addItem(item);
}

void RemoveItemsCommand::undo()
{
    for (auto item : m_items)
        m_sceneRemovedItems->removeItem(item);

    for (auto item : m_items)
        m_scene->addItem(item);
}

MoveItemCommand::MoveItemCommand(MyGraphicsItem *item, QPointF newPos, QPointF oldPos)
    : m_bFirstRedo(true),
      m_item(item),
      m_newPos(newPos),
      m_oldPos(oldPos)

{

}

void MoveItemCommand::redo()
{
    if (!m_bFirstRedo)
    {
        m_item->setPos(m_newPos);
    }

    m_bFirstRedo = false;
}

void MoveItemCommand::undo()
{
    m_item->setPos(m_oldPos);
}

InsertItemCommand::InsertItemCommand(QGraphicsItem *item, QGraphicsScene *scene, QPointF pos)
    : m_item(item),
      m_scene(scene),
      m_pos(pos)
{

}

void InsertItemCommand::redo()
{
    m_scene->addItem(m_item);
    m_item->setPos(m_pos);
}

void InsertItemCommand::undo()
{
    m_scene->removeItem(m_item);
}

