/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "treewidgetaction.h"
#include "ui_treewidgetaction.h"

#include <QPushButton>
#include <QAction>
#include <QSizePolicy>

TreeWidgetAction::TreeWidgetAction(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TreeWidgetAction)
{
    ui->setupUi(this);
}

TreeWidgetAction::~TreeWidgetAction()
{
    delete ui;
}

void TreeWidgetAction::setText(QString label)
{
    ui->label->setText(label);
}

void TreeWidgetAction::addAction(QAction *action, bool after)
{
    QPushButton *btn = new QPushButton(action->text());
    btn->setIcon(action->icon());
    btn->setMinimumSize(15, 15);
    btn->setMaximumSize(15, 15);
    btn->setIconSize(QSize(15, 15));
    btn->setStyleSheet("border: 0px solid transparent; background-color: transparent;");
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_mapActions[btn] = action;

    if (after)
        ui->layoutActions->addWidget(btn);
    else
        ui->layoutActionsPrev->addWidget(btn);

    connect(btn, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void TreeWidgetAction::onClicked()
{
    auto act = m_mapActions[sender()];

    act->trigger();
}
