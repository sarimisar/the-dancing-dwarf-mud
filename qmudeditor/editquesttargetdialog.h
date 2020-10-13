/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef EDITQUESTTARGET_H
#define EDITQUESTTARGET_H

#include <QDialog>

#include "editormap.h"

namespace Ui {
class EditQuestTargetDialog;
}

class EditQuestTargetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditQuestTargetDialog(const QVector<EditorMap *> &maps, QWidget *parent = nullptr);
    ~EditQuestTargetDialog();

    void setData(const EditorMap::QuestData::TargetData& data);
    EditorMap::QuestData::TargetData data() const;

private slots:
    void on_pushButtonNpcToTalkToMapID_clicked();
    void on_pushButtonNpcToTalkToNpcID_clicked();
    void on_pushButtonNpcToKillMapID_clicked();
    void on_pushButtonNpcToKillNpcID_clicked();

private:
    Ui::EditQuestTargetDialog *ui;

    QVector<EditorMap *> m_vMaps;
};

#endif // EDITQUESTTARGET_H
