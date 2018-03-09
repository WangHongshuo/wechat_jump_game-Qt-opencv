#include "fixparameterdialog.h"
#include "ui_fixparameterdialog.h"
#include <QStandardItemModel>
#include <QDebug>

FixParameterDialog::FixParameterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FixParameterDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    initializedTableViewParameterInfo();
}

FixParameterDialog::~FixParameterDialog()
{
    delete ui;
}

void FixParameterDialog::on_comboBoxStep_currentIndexChanged(int index)
{
    step = steps[index];
}

void FixParameterDialog::on_pushButtonInc_clicked()
{
    emit sendAdjustedCorrectionValue(step,11);
}

void FixParameterDialog::on_pushButtonDec_clicked()
{
    emit sendAdjustedCorrectionValue(-step,11);
}

void FixParameterDialog::initializedTableViewParameterInfo()
{
    QStandardItemModel *model = new QStandardItemModel();
    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal,"    ");
    model->setHeaderData(1,Qt::Horizontal,"Last");
    model->setHeaderData(2,Qt::Horizontal,"Next");
    model->setRowCount(2);
    model->setItem(0,0,new QStandardItem("distance"));
    model->setItem(1,0,new QStandardItem("correction"));
    ui->tableViewParameterInfo->setModel(model);
    for(int i=0;i<3;i++)
        ui->tableViewParameterInfo->setColumnWidth(i,90);
}
