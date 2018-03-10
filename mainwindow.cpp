#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include "QImageMatConvert/mat_qimage_convert.h"
#include "ImageWidget-Qt/ImageWidget.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


// ANSI

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 只接受数字
    QRegExp rx("^(-?[0]|-?[1-9][0-9]{0,5})(?:\\.\\d{1,4})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);

    ui->lineEditDistanceParameter->setValidator(pReg);
    ui->lineEditDistanceParameter->setText(QString::number(jumpGame.getPressScreenTimeParameter()));

    ui->pushButtonSwitchAutoJump->setEnabled(false);

    ui->widgetShowImage->setEnableDragImage(false);
    ui->widgetShowImage->setEnableZoomImage(false);
    ui->widgetShowImage->setEnableRecordLastParameters(true);
    ui->widgetShowTemplate->setEnableOnlyShowImage(true);
    ui->widgetShowImage->setEnableSendLeftClickedPosInImage(true);

    qImageTemplate.load(QCoreApplication::applicationDirPath()+"/template.png");
    if(qImageTemplate.isNull())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Template initialization failed!"));
        msgBox.exec();
        on_pushButtonLoadTemplate_clicked();
    }
    else
    {
        ui->widgetShowTemplate->setImageWithPointer(&qImageTemplate);
        matTemplate = QImage2Mat_with_pointer(qImageTemplate);
        jumpGame.setTemplateImage(matTemplate);
    }

    connect(ui->widgetShowImage,SIGNAL(sendLeftClickedPosInImage(int,int)),this,SLOT(receiveWidgetShowImageClickedPosInImage(int,int)));
    connect(&controller,SIGNAL(sendMatScreenshotImage(cv::Mat)),this,SLOT(receiveMatScreenshotAndProcess(cv::Mat)));
    connect(&controller,SIGNAL(sendJumpControllerMessage(QString)),this,SLOT(receiveJumpControllerMessage(QString)));

    adbFilePath = QCoreApplication::applicationDirPath() + "\\adb\\adb";
    controller.initializeAdbService(adbFilePath);
    controller.setCheatMode(true);
    if(!jumpGame.loadInifile((QCoreApplication::applicationDirPath()+"/default.ini").toStdString()))
    {
        QMessageBox msgBox;
        msgBox.setText(tr("default.ini load fail."));
        msgBox.exec();
    }
    jumpGame.setEnableOutputLog(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receiveAdjustedParameterValue(double delta, int index)
{
    jumpGame.fixCorrectionsBuffer(delta,index);
}

void MainWindow::showImage(QImage &src)
{
    if(!src.isNull())
        ui->widgetShowImage->setImageWithPointer(&src);
}

void MainWindow::showImage(cv::Mat &src)
{
    if(src.data)
    {
        qImageScreenShot = Mat2QImage_with_data(src);
        ui->widgetShowImage->setImageWithPointer(&qImageScreenShot);
    }
}

void MainWindow::receiveWidgetShowImageClickedPosInImage(int x, int y)
{
    if(ui->radioButtonManualJump->isChecked())
    {
        jumpGame.setLeftClickedPos(x,y);
        showImage(jumpGame.outputImage);
        updateLables();
    }
}

void MainWindow::updateLables()
{
    ui->labelX1Y1->setText(QString::number(jumpGame.manLocationX())+" , "+QString::number(jumpGame.manLocationY()));
    ui->labelX2Y2->setText(QString::number(jumpGame.blockLocationX())+" , "+QString::number(jumpGame.blockLocationY()));
    ui->labelDistance->setText(QString::number(jumpGame.jumpDistance()));
    ui->labelParameter->setText(ui->lineEditDistanceParameter->text());
}

void MainWindow::on_pushButtonJump_clicked()
{
    if(!controller.isAdbServiceInitializatedFlag())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Can't find adb.exe!"));
        msgBox.exec();
    }
    else
    {
        controller.jumpAction(jumpGame.getPressScreenTime());
    }
}

void MainWindow::on_pushButtonFindAdb_clicked()
{
    adbFilePath = QFileDialog::getOpenFileName(this,tr("Open adb.exe"),
                                                        QCoreApplication::applicationDirPath(),
                                                        tr("adb file(adb.exe)"));
    qDebug() << adbFilePath;
    adbFilePath.chop(4);
    adbFilePath = QDir::toNativeSeparators(adbFilePath);
    controller.refreshAdbService(adbFilePath);
}

void MainWindow::on_pushButtonGetScreenshotImage_clicked()
{
    if(!controller.isDetectedDeviceFlag())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Adb initialization failed!"));
        msgBox.exec();
    }
    else
    {
        controller.getMatScreenshotImage();
    }
}

void MainWindow::on_pushButtonRefreshAdb_clicked()
{
    controller.refreshAdbService();
}

void MainWindow::on_pushButtonTest_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("open image"),
                                                    "F:/1/ ",
                                                    tr("Image File(*.bmp *.jpg *.png"));
    if(filePath.isEmpty() | filePath.isNull())
    {
        qDebug() << "filePath error!";
        return;
    }
    else
        qImageScreenShot.load(filePath);
    if(qImageScreenShot.isNull())
    {
        qDebug() << "image load fail!";
        return;
    }
    else
    {
        matScreenShot = QImage2Mat_with_data(qImageScreenShot);
        jumpGame.setInputImage(matScreenShot);
        showImage(jumpGame.outputImage);
        updateLables();
    }
}

void MainWindow::on_pushButtonLoadTemplate_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("open image"),
                                                    QApplication::applicationDirPath(),
                                                    tr("Image File(*.bmp *.jpg *.png"));
    if(filePath.isNull() || filePath.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Template load failed!"));
        msgBox.exec();
        on_pushButtonLoadTemplate_clicked();
    }
    else
    {
        qImageTemplate.load(filePath);
        if(qImageTemplate.isNull())
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Template load failed!"));
            msgBox.exec();
            on_pushButtonLoadTemplate_clicked();
        }
         else
        {
            ui->widgetShowTemplate->setImageWithPointer(&qImageTemplate);
            matTemplate = QImage2Mat_with_pointer(qImageTemplate);
            jumpGame.setTemplateImage(matTemplate);
        }
    }

}

void MainWindow::on_pushButtonUpdateProcessedImage_clicked()
{
    if(jumpGame.isLoadInputImage())
    {
        jumpGame.update();
        showImage(jumpGame.outputImage);
        updateLables();
    }
}


void MainWindow::on_radioButtonManualJump_clicked()
{
    isAutoJumpModeSelected = false;
    ui->pushButtonSwitchAutoJump->setEnabled(false);
    ui->pushButtonGetScreenshotImage->setEnabled(true);
    ui->pushButtonJump->setEnabled(true);
    if(isAutoJumpActived)
        on_pushButtonSwitchAutoJump_clicked();
}

void MainWindow::on_radioButtonAutoJump_clicked()
{
    isAutoJumpModeSelected = true;
    ui->pushButtonSwitchAutoJump->setEnabled(true);
    ui->pushButtonGetScreenshotImage->setEnabled(false);
    ui->pushButtonJump->setEnabled(false);
}

void MainWindow::on_pushButtonSwitchAutoJump_clicked()
{
    if(!isAutoJumpActived)
    {
        ui->pushButtonSwitchAutoJump->setText("Stop(S)");
        ui->pushButtonSwitchAutoJump->setShortcut(Qt::Key_S);
        isAutoJumpActived = true;
        controller.startAutoJumpLoop();
    }
    else
    {
        isAutoJumpActived = false;
        ui->pushButtonSwitchAutoJump->setText("Start(S)");
        ui->pushButtonSwitchAutoJump->setShortcut(Qt::Key_S);
        controller.stopAutoJumpLoop();
    }
}

void MainWindow::on_pushButtonTestSaveInputImage_clicked()
{
    QImage temp = Mat2QImage_with_data(jumpGame.returnInputImage());
    QString filename = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                    QCoreApplication::applicationDirPath(),
                                                    tr("Images (*.png *.xpm *.jpg *.tiff *.bmp)"));
    if(!filename.isEmpty() || !filename.isNull())
        temp.save(filename);
}

void MainWindow::on_lineEditDistanceParameter_editingFinished()
{
    jumpGame.setPressScreenTimeParameter(ui->lineEditDistanceParameter->text().toDouble());
    ui->labelParameter->setText(QString::number(jumpGame.getPressScreenTimeParameter()));
}

void MainWindow::receiveMatScreenshotAndProcess(cv::Mat img)
{
    matScreenShot = img;
    if(matScreenShot.data)
    {
        jumpGame.setInputImage(matScreenShot);
        emit sendCurrentJumpLog(jumpGame.getCurrentJumpLog());
        showImage(jumpGame.outputImage);
        updateLables();
        if(isAutoJumpModeSelected &&isAutoJumpActived)
            controller.jumpAction(jumpGame.getPressScreenTime());
    }
}

void MainWindow::receiveJumpControllerMessage(QString msg)
{
    ui->statusBar->showMessage(msg);
}

void MainWindow::on_pushButtonLoadIniFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("open file"),
                                                    QApplication::applicationDirPath(),
                                                    tr("ini File (*.ini)"));
    if(!filePath.isNull() && !filePath.isEmpty())
    {
        std::string strFilePath = filePath.toStdString();
        if(!jumpGame.loadInifile(strFilePath))
            ui->statusBar->showMessage("ini file load error and parameters will not be changed!");
        else
            ui->statusBar->showMessage("ini file load successful!");
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr(".ini file load failed!"));
        msgBox.exec();
    }
}

void MainWindow::on_pushButtonFixPatameters_clicked()
{
    ui->pushButtonFixPatameters->setEnabled(false);
    FixParameterDialog *a = new FixParameterDialog(this);
    connect(a,SIGNAL(sendAdjustedCorrectionValue(double,int)),
            this,SLOT(receiveAdjustedParameterValue(double,int)),
            Qt::UniqueConnection);
    connect(a,SIGNAL(destroyed(QObject*)),
            this,SLOT(setPushButtonFixParametersEnable()),
            Qt::UniqueConnection);
            a->show();
}

void MainWindow::setPushButtonFixParametersEnable()
{
    ui->pushButtonFixPatameters->setEnabled(true);
}
