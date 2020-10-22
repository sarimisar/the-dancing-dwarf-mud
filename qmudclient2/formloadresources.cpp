#include "formloadresources.h"
#include "ui_formloadresources.h"

FormLoadResources::FormLoadResources(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLoadResources)
{
    ui->setupUi(this);

    ui->progressBar->setMaximum(100);

    connect(this, &FormLoadResources::status, this, &FormLoadResources::onStatus);
    connect(this, &FormLoadResources::complete, this, &FormLoadResources::onComplete);
}

FormLoadResources::~FormLoadResources()
{
    delete ui;
}

void FormLoadResources::onStatus(int percentage)
{
    ui->progressBar->setValue(percentage);
}

void FormLoadResources::onComplete()
{
    QCoreApplication::exit(0);
}
