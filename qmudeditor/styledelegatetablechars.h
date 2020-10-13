/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef STYLEDELEGATETABLECHARS_H
#define STYLEDELEGATETABLECHARS_H

#include <QStyledItemDelegate>
#include <QTableView>

class StyleDelegateTableChars : public QStyledItemDelegate
{
public:
    StyleDelegateTableChars(QTableView *table, QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QTableView* m_ptrTable;
};

#endif // STYLEDELEGATETABLECHARS_H
