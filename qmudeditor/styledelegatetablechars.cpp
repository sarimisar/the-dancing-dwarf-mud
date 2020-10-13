/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "styledelegatetablechars.h"
#include "tablemodel.h"

#include <QApplication>
#include <QTableWidgetItem>

StyleDelegateTableChars::StyleDelegateTableChars(QTableView *table, QObject *parent) :
    QStyledItemDelegate(parent),
    m_ptrTable(table)
{

}

void StyleDelegateTableChars::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem subop = option;
    QPalette pal = qApp->palette();

    bool selected = m_ptrTable->selectionModel()->isSelected(index);

    if (selected)
    {
        TableModel* model = qobject_cast<TableModel*>(m_ptrTable->model());

        pal.setColor(QPalette::Highlight, Qt::black);
        pal.setColor(QPalette::WindowText, model->data(index, Qt::ForegroundRole).value<QColor>());
        pal.setColor(QPalette::HighlightedText, pal.color(QPalette::WindowText));
        pal.setColor(QPalette::Text, pal.color(QPalette::WindowText));

        subop.state |= QStyle::State_Selected;
        subop.palette = pal;
    }

    QStyledItemDelegate::paint(painter, subop, index);	// Note: subop.
}
