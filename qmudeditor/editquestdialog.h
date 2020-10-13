/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef EDITQUESTDIALOG_H
#define EDITQUESTDIALOG_H

#include <QDialog>

#include "editormap.h"

namespace Ui {
class EditQuestDialog;
}

class EditQuestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditQuestDialog(const QVector<EditorMap *> &maps, EditorMap *currentMap, QWidget *parent = nullptr);
    ~EditQuestDialog();

    void setData(const EditorMap::QuestData& data);
    EditorMap::QuestData data() const;

private slots:
    void on_pushButtonAddQuestTarget_clicked();
    void on_tableWidgetQuestTargets_cellDoubleClicked(int row, int column);
    void on_pushButtonGiveNpcSelectMapID_clicked();
    void on_pushButtonGiveNpcSelectNpcID_clicked();
    void on_pushButtonCompleteNpcSelectMapID_clicked();
    void on_pushButtonCompleteNpcSelectNpcID_clicked();
    void on_pushButtonGiftItem_clicked();
    void on_pushButtonGiftItemMap_clicked();

private:
    void addTarget(const EditorMap::QuestData::TargetData &data);

private:
    Ui::EditQuestDialog *ui;

    QVector<EditorMap *> m_vMaps;
    EditorMap *m_ptrCurrentMap;
};

#endif // EDITQUESTDIALOG_H
