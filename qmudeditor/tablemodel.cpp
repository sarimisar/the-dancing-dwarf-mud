/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "tablemodel.h"
#include "mapeditordialog.h"

TableModel::TableModel(MapEditorDialog *mapEditor, QObject *parent) :
    QAbstractTableModel(parent),
    m_iSelectedRow(-1),
    m_iSelectedColumn(-1),
    m_iSelectionModeSelectedRow(-1),
    m_iSelectionModeSelectedColumn(-1),
    m_mapEditor(mapEditor),
    m_bBGShowRooms(false)

{
    m_mapData.push_back(QVector<QChar>(1));
    m_mapData[0][0] = ' ';
}

bool TableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    if (m_mapData.size() < row)
        return false;

    beginInsertRows(QModelIndex(), row, row + count - 1);

    QVector<QChar> newRow;
    if (!m_mapData.isEmpty())
        newRow.resize(m_mapData[0].size());
    else
        newRow.resize(0);

    newRow.fill(QChar(' '));

    m_mapData.insert(row, count, newRow);

    endInsertRows();

    return true;
}

bool TableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    if (!m_mapData.isEmpty() && m_mapData[0].size() < column)
        return false;

    beginInsertColumns(QModelIndex(), column, column + count - 1);

    for (auto &row : m_mapData)
        row.insert(column, count, QChar(' '));

    endInsertColumns();

    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    if (m_mapData.size() < row+count)
        return false;

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    m_mapData.remove(row, count);

    endRemoveRows();

    return true;
}

bool TableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    if (m_mapData.isEmpty() || m_mapData[0].size() < column+count)
        return false;

    beginRemoveColumns(QModelIndex(), column, column + count - 1);

    for (auto& row : m_mapData)
        row.remove(column, count);

    endRemoveColumns();

    return true;
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_mapData.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_mapData.isEmpty())
        return 0;
    else
        return m_mapData[0].size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    auto val = m_mapEditor->m_mapChars.find(m_mapData[index.row()][index.column()]);

    if (val == m_mapEditor->m_mapChars.end())
        return QVariant();

    if (role == Qt::DisplayRole)
        return val->value;
    else if (role == Qt::UserRole)
        return val->key;
    else if (role == Qt::ForegroundRole)
        return val->color;
    else if (role == Qt::BackgroundRole || role == Qt::BackgroundColorRole)
    {
        if (index.row() == m_iSelectionModeSelectedRow && index.column() == m_iSelectionModeSelectedColumn)
            return QColor(100, 100, 255);
        else if (index.row() == m_iSelectedRow && index.column() == m_iSelectedColumn)
            return QColor(100, 100, 100);
        else
        {
            if (m_bBGShowRooms)
            {
                if (val->is_room)
                    return QColor(0, 50, 0);
                else
                    return QColor(50, 0, 0);
            }
            else
                return QColor(0, 0, 0);
        }
    }
    else if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    else
        return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() >= 0 && index.row() < m_mapData.size() && !m_mapData.isEmpty() && index.column() < m_mapData[0].size())
    {
        if (role == Qt::EditRole)
        {
            m_mapData[index.row()][index.column()] = value.toChar();
            emit dataChanged(index, index);
        }

        return true;
    }

    return false;
}

void TableModel::setMap(QVector<QVector<QChar> > map)
{
    if (map.isEmpty() || map[0].isEmpty())
    {
        map.clear();
        map.push_back(QVector<QChar>(1));
        map[0][0] = ' ';
    }

    int prevWidth = m_mapData.isEmpty() ? 0 : m_mapData[0].size();
    int prevHeight = m_mapData.size();

    int currWidth = map.isEmpty() ? 0 : map[0].size();
    int currHeight = map.size();

    if (currWidth > prevWidth)
        insertColumns(0, currWidth - prevWidth);
    else if (currWidth < prevWidth)
        removeColumns(0, prevWidth - currWidth);

    if (currHeight > prevHeight)
        insertRows(0, currHeight - prevHeight);
    else if (m_mapData.size() < prevHeight)
        removeRows(0, prevHeight - currHeight);

    m_mapData = map;

    emit dataChanged(index(0, 0), index(currHeight - 1, currWidth - 1));
}

QVector<QVector<QChar> > TableModel::map() const
{
    return m_mapData;
}

void TableModel::setBGShowRooms(bool show)
{
    m_bBGShowRooms = show;

    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void TableModel::setSelectedCell(int row, int column)
{
    m_iSelectedRow = row;
    m_iSelectedColumn = column;

    QModelIndex index = this->index(row, column);

    emit dataChanged(index, index);
}

void TableModel::setSelectionModeSelectedCell(int row, int column)
{
    m_iSelectionModeSelectedRow = row;
    m_iSelectionModeSelectedColumn = column;

    QModelIndex index = this->index(row, column);

    emit dataChanged(index, index);
}

DrawCommand::DrawCommand(QVector<DrawCommand::DrawData> data, QAbstractItemModel *model) :
    QUndoCommand(),
    m_model(model)
{
    for (auto elem : data)
    {
        PrivateDrawData pData;
        pData.row = elem.index.row();
        pData.col = elem.index.column();
        pData.valueNew = elem.value;
        pData.valueOld = elem.index.data(Qt::UserRole);
        m_drawData.push_back(pData);
    }
}

void DrawCommand::redo()
{
    for (int i=0; i<m_drawData.size(); ++i)
    {
        PrivateDrawData data = m_drawData[i];
        m_model->setData(m_model->index(data.row, data.col), data.valueNew);
    }
}

void DrawCommand::undo()
{
    for (int i=m_drawData.size()-1; i>=0; --i)
    {
        PrivateDrawData data = m_drawData[i];
        m_model->setData(m_model->index(data.row, data.col), data.valueOld);
    }
}

FillCommand::FillCommand(QModelIndex index, QChar key, QAbstractItemModel *model)
{
    m_model = model;
    m_key = key;
    m_currKey = model->data(index, Qt::UserRole).toChar();
    m_row = index.row();
    m_col = index.column();
}

void FillCommand::floodFill(int row, int col)
{
    if (row < 0 || row >= m_model->rowCount() ||
        col < 0 || col >= m_model->columnCount() ||
        m_model->data(m_model->index(row, col), Qt::UserRole).toChar() != m_currKey ||
            m_model->data(m_model->index(row, col), Qt::UserRole).toChar() == m_key)
        return;

    m_model->setData(m_model->index(row, col), m_key);

    floodFill(row+1, col);
    floodFill(row-1, col);
    floodFill(row, col+1);
    floodFill(row, col-1);
}

void FillCommand::redo()
{
    m_mapData = qobject_cast<TableModel*>(m_model)->m_mapData;

    floodFill(m_row, m_col);
}

void FillCommand::undo()
{
    qobject_cast<TableModel*>(m_model)->setMap(m_mapData);
}

InsertRowCommand::InsertRowCommand(int row, int count, QAbstractItemModel *model, QTableView *view)
{
    m_row = row;
    m_count = count;
    m_model = model;
    m_view = view;
}

void InsertRowCommand::redo()
{
    m_model->insertRows(m_row, m_count);

    for (int i=m_row; i < m_model->rowCount(); i++)
        m_view->setRowHeight(i, 16);
}

void InsertRowCommand::undo()
{
    m_model->removeRows(m_row, m_count);
}

InsertColumnCommand::InsertColumnCommand(int column, int count, QAbstractItemModel *model, QTableView *view)
{
    m_column = column;
    m_count = count;
    m_model = model;
    m_view = view;
}

void InsertColumnCommand::redo()
{
    m_model->insertColumns(m_column, m_count);

    for (int i=m_column; i < m_model->columnCount(); i++)
        m_view->setColumnWidth(i, 9);
}

void InsertColumnCommand::undo()
{
    m_model->removeColumns(m_column, m_count);
}

RemoveRowCommand::RemoveRowCommand(int row, int count, QAbstractItemModel *model, QTableView *view)
{
    if (model->rowCount() > count)
    {
        m_row = row;
        m_count = count;
        m_model = model;
        m_view = view;
    }
    else
    {
        m_row = 0;
        m_count = 0;
        m_model = model;
        m_view = view;
    }
}

void RemoveRowCommand::redo()
{
    if (m_count > 0)
    {
        m_mapData = qobject_cast<TableModel*>(m_model)->m_mapData;
        m_model->removeRows(m_row, m_count);
    }
}

void RemoveRowCommand::undo()
{
    if (m_count > 0)
    {
        qobject_cast<TableModel*>(m_model)->setMap(m_mapData);

        for (int i=0; i<m_view->model()->columnCount(); i++)
            m_view->setColumnWidth(i, 9);

        for (int i=0; i<m_view->model()->rowCount(); i++)
            m_view->setRowHeight(i, 16);
    }
}

RemoveColumnCommand::RemoveColumnCommand(int column, int count, QAbstractItemModel *model, QTableView *view)
{
    if (model->columnCount() > count)
    {
        m_column = column;
        m_count = count;
        m_model = model;
        m_view = view;
    }
    else
    {
        m_column = 0;
        m_count = 0;
        m_model = model;
        m_view = view;
    }
}

void RemoveColumnCommand::redo()
{
    if (m_count > 0)
    {
        m_mapData = qobject_cast<TableModel*>(m_model)->m_mapData;
        m_model->removeColumns(m_column, m_count);
    }
}

void RemoveColumnCommand::undo()
{
    if (m_count > 0)
    {
        qobject_cast<TableModel*>(m_model)->setMap(m_mapData);

        for (int i=0; i<m_view->model()->columnCount(); i++)
            m_view->setColumnWidth(i, 9);

        for (int i=0; i<m_view->model()->rowCount(); i++)
            m_view->setRowHeight(i, 16);
    }
}
