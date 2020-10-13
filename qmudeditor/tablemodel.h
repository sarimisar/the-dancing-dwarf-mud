/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QUndoCommand>
#include <QTableView>

class MapEditorDialog;

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

    friend class DrawCommand;
    friend class StyleDelegateTableChars;
    friend class RemoveRowCommand;
    friend class RemoveColumnCommand;
    friend class FillCommand;

public:
    TableModel(MapEditorDialog* mapEditor, QObject *parent = nullptr);

    void setSelectedCell(int row, int column);
    void setSelectionModeSelectedCell(int row, int column);
    void setMap(QVector<QVector<QChar>> map);

    QVector<QVector<QChar>> map() const;

    void setBGShowRooms(bool show);

private:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

private:
    QVector<QVector<QChar> > m_mapData;
    int m_iSelectedRow;
    int m_iSelectedColumn;
    int m_iSelectionModeSelectedRow;
    int m_iSelectionModeSelectedColumn;

    MapEditorDialog *m_mapEditor;

    bool m_bBGShowRooms;
};

class DrawCommand : public QUndoCommand
{
public:
    struct DrawData
    {
        QVariant value;
        QModelIndex index;
    };

    DrawCommand(QVector<DrawData> data, QAbstractItemModel *model);

    void redo();
    void undo();

private:
    struct PrivateDrawData
    {
        QVariant valueNew;
        QVariant valueOld;
        int row;
        int col;
    };

    QAbstractItemModel *m_model;
    QVector<PrivateDrawData> m_drawData;
};

class FillCommand : public QUndoCommand
{
public:
    FillCommand(QModelIndex index, QChar key, QAbstractItemModel *model);

    void redo();
    void undo();

private:
    QAbstractItemModel *m_model;
    int m_row;
    int m_col;
    QChar m_key;
    QChar m_currKey;

    QVector<QVector<QChar> > m_mapData;
    void floodFill(int row, int col);
};

class InsertRowCommand : public QUndoCommand
{
public:
    InsertRowCommand(int row, int count, QAbstractItemModel *model, QTableView* view);

    void redo();
    void undo();

private:
    QAbstractItemModel *m_model;
    QTableView *m_view;
    int m_row;
    int m_count;
};

class RemoveRowCommand : public QUndoCommand
{
public:
    RemoveRowCommand(int row, int count, QAbstractItemModel *model, QTableView* view);

    void redo();
    void undo();

private:
    QAbstractItemModel *m_model;
    QTableView *m_view;
    int m_row;
    int m_count;

    QVector<QVector<QChar> > m_mapData;
};

class InsertColumnCommand : public QUndoCommand
{
public:
    InsertColumnCommand(int column, int count, QAbstractItemModel *model, QTableView* view);

    void redo();
    void undo();

private:
    QAbstractItemModel *m_model;
    QTableView *m_view;
    int m_column;
    int m_count;
};

class RemoveColumnCommand : public QUndoCommand
{
public:
    RemoveColumnCommand(int column, int count, QAbstractItemModel *model, QTableView* view);

    void redo();
    void undo();

private:
    QAbstractItemModel *m_model;
    QTableView *m_view;
    int m_column;
    int m_count;

    QVector<QVector<QChar> > m_mapData;
};

#endif // TABLEMODEL_H
