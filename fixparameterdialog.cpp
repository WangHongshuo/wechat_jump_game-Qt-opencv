#include "fixparameterdialog.h"
#include "ui_fixparameterdialog.h"
#include <QDebug>

FixParameterDialog::FixParameterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FixParameterDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
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
