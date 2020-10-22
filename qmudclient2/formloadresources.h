#ifndef FORMLOADRESOURCES_H
#define FORMLOADRESOURCES_H

#include <QWidget>

namespace Ui {
class FormLoadResources;
}

class FormLoadResources : public QWidget
{
    Q_OBJECT

public:
    explicit FormLoadResources(QWidget *parent = nullptr);
    ~FormLoadResources();

signals:
    void status(double percentage);
    void complete();

private slots:
    void onStatus(int percentage);
    void onComplete();

private:
    Ui::FormLoadResources *ui;
};

#endif // FORMLOADRESOURCES_H
