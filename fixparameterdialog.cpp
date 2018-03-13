#include "fixparameterdialog.h"
#include "ui_fixparameterdialog.h"
#include <QStandardItemModel>
#include <QDebug>
#include <QFileDialog>
#include <QPoint>

FixParameterDialog::FixParameterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FixParameterDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    initializedTableViewParameterInfo();
    QPalette palette;
    palette.setColor(QPalette::Text,Qt::red);
    ui->lineEditAdjustInfo->setPalette(palette);
}

FixParameterDialog::~FixParameterDialog()
{
    delete model;
    delete ui;
}

void FixParameterDialog::receiveCurrentJumpLog(double *log)
{
    // index distance correction
    lastIndex = int(log[0]);
    nextIndex = int(log[3]);
    qDebug() << lastIndex << nextIndex;
    QModelIndex index;
    for(int i=0;i<3;i++)
    {
        for(int j=1;j<3;j++)
        {
            index = model->index(i,j);
            model->setData(index,QVariant::fromValue(log[(j-1)*3+i]));
        }
    }

}

void FixParameterDialog::on_comboBoxStep_currentIndexChanged(int index)
{
    step = steps[index];
}

void FixParameterDialog::on_pushButtonInc_clicked()
{
    if(lastIndex > 0)
        ui->lineEditAdjustInfo->setText("Adjusted correction "+
                                         QString::number(lastIndex)+
                                         "        " +QString::number(step));
    else
        ui->lineEditAdjustInfo->setText("Index " + QString::number(lastIndex)+
                                        " is invalid!");
    emit sendAdjustedCorrectionValue(step,lastIndex);
}

void FixParameterDialog::on_pushButtonDec_clicked()
{
    if(lastIndex > 0)
        ui->lineEditAdjustInfo->setText("Adjusted correction "+
                                         QString::number(lastIndex)+
                                         "        " +QString::number(-step));
    else
        ui->lineEditAdjustInfo->setText("Index " + QString::number(lastIndex)+
                                        " is invalid!");
    emit sendAdjustedCorrectionValue(-step,lastIndex);
}

void FixParameterDialog::initializedTableViewParameterInfo()
{
    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal,"    ");
    model->setHeaderData(1,Qt::Horizontal,"Last");
    model->setHeaderData(2,Qt::Horizontal,"Next");
    model->setRowCount(3);
    model->setItem(0,0,new QStandardItem("index"));
    model->setItem(1,0,new QStandardItem("distance"));
    model->setItem(2,0,new QStandardItem("correction"));
    for(int i=0;i<3;i++)
        for(int j=1;j<3;j++)
            model->setItem(i,j,new QStandardItem("NULL"));
    ui->tableViewParameterInfo->setModel(model);
    for(int i=0;i<3;i++)
        ui->tableViewParameterInfo->setColumnWidth(i,90);
}

void FixParameterDialog::on_pushButtonSaveAsIni_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    QCoreApplication::applicationDirPath(),
                                                    tr("INI (*.ini)"));
    if(!filename.isEmpty() && !filename.isNull())
        emit sendSaveCorrectionsAction(filename);
}
