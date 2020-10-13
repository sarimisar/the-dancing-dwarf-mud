/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef TREEWIDGETACTION_H
#define TREEWIDGETACTION_H

#include <QWidget>
#include <QMap>

namespace Ui {
class TreeWidgetAction;
}

class TreeWidgetAction : public QWidget
{
    Q_OBJECT

public:
    explicit TreeWidgetAction(QWidget *parent = nullptr);
    ~TreeWidgetAction();

    void setText(QString label);
    void addAction(QAction *action, bool after = true);

private slots:
    void onClicked();

private:
    Ui::TreeWidgetAction *ui;

    QMap<QObject*, QAction*> m_mapActions;
};

#endif // TREEWIDGETACTION_H
