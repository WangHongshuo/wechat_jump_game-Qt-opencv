#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QBuffer>
#include <QImageReader>
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
    timerAuToJump = new QTimer(this);
    // 只接受数字
    QRegExp rx("^(-?[0]|-?[1-9][0-9]{0,5})(?:\\.\\d{1,4})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    adbFilePath = QCoreApplication::applicationDirPath() + "\\adb\\adb";
    ui->lineEditDistanceParameter->setValidator(pReg);
    ui->lineEditDistanceParameter->setText(QString::number(jumpGame.getPressScreenTimeParameter()));

    ui->pushButtonSwitchAutoJump->setEnabled(false);

    ui->widgetShowImage->setEnableDragImage(false);
    ui->widgetShowImage->setEnableZoomImage(false);
    ui->widgetShowImage->setEnableRecordLastParameters(true);
    ui->widgetShowTemplate->setEnableOnlyShowImage(true);
    ui->widgetShowImage->setEnableSendLeftClickedPosInImage(true);
    connect(ui->widgetShowImage,SIGNAL(sendLeftClickedPosInImage(int,int)),this,SLOT(receiveWidgetShowImageClickedPosInImage(int,int)));
    initializeAdbServer();
    connect(&getScreenshotProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](){getImageFromStdOutputAndProcessImage();});

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
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::initializeAdbServer()
{
    adbProcess.start(adbFilePath + " devices");
    adbProcess.waitForFinished();
    adbProcess.start(adbFilePath + " devices");
    if(!adbProcess.waitForFinished())
    {
        ui->statusBar->showMessage("adb error!");
        isAdbInitializated = false;
    }
    else
    {
        QString output =QString::fromLocal8Bit(adbProcess.readAllStandardOutput());
        output = output.simplified();
        if(output.startsWith("List of devices attached"))
        {
            if(output.mid(25).isEmpty() || output.mid(25).isNull())
            {
                ui->statusBar->showMessage("No Devices!");
                isAdbInitializated = false;
            }
            else
            {
                ui->statusBar->showMessage("Find: " + output.mid(25));
                isAdbInitializated = true;
            }
        }
        else
        {
            ui->statusBar->showMessage("adb error!");
            isAdbInitializated = false;
        }
    }
}

void MainWindow::receiveWidgetShowImageClickedPosInImage(int x, int y)
{
    if(ui->radioButtonManualJump->isChecked())
    {
        jumpGame.setLeftClickedPos(x,y);
        showImage(jumpGame.outputImage);
        ui->labelX1Y1->setText(QString::number(jumpGame.manLocationX())+" , "+QString::number(jumpGame.manLocationY()));
        ui->labelX2Y2->setText(QString::number(x)+" , "+QString::number(y));
        ui->labelDistance->setText(QString::number(jumpGame.jumpDistance()));
        ui->labelParameter->setText(ui->lineEditDistanceParameter->text());
    }
}

void MainWindow::timerAuToJumpTimeoutEvent()
{
    timerAuToJump->stop();
    on_pushButtonGetScreenshotImage_clicked();
}

void MainWindow::reloadAutoJumpTimer()
{
    ui->statusBar->showMessage("Standby.");
    if(isAutoJump && isAutoJumpMode)
    {
        timerAuToJump->setInterval(timerAuToJumpDelay);
        timerAuToJump->start();
    }
    else if(isAutoJumpMode)
        ui->pushButtonSwitchAutoJump->setEnabled(true);
}

void MainWindow::getImageFromStdOutputAndProcessImage()
{
    if(isAutoJumpMode && !isAutoJump)
    {
        ui->pushButtonSwitchAutoJump->setEnabled(true);
    }
    else
    {
        QByteArray data;
        data = getScreenshotProcess.readAll();
        //        qDebug() << data.length();

        if(getScreenshotMode == 0)
            data = data.replace("\r\n","\n");
        else if(getScreenshotMode == 1)
            data = data.replace("\r\r\n","\n");
        //        qDebug() << data.length();
        qDebug() << getScreenshotMode;
        // to Mat
        std::vector<uchar> buffer(data.begin(),data.end());
        matScreenShot = cv::imdecode(buffer,CV_LOAD_IMAGE_COLOR);

        // to QImage
        //        QBuffer buffer(&data);
        //        QImageReader reader(&buffer);
        //        reader.setFormat("PNG");
        //        qImageScreenShot = reader.read();
        ui->statusBar->showMessage("Processing...");
        if(matScreenShot.data )
        {
            isGetImage = true;
            jumpGame.setInputImage(matScreenShot);
            showImage(jumpGame.outputImage);
            ui->labelX1Y1->setText(QString::number(jumpGame.manLocationX())+" , "+QString::number(jumpGame.manLocationY()));
            ui->labelX2Y2->setText(QString::number(jumpGame.blockLocationX())+" , "+QString::number(jumpGame.blockLocationY()));
            ui->labelDistance->setText(QString::number(jumpGame.jumpDistance()));
            ui->labelParameter->setText(ui->lineEditDistanceParameter->text());
            ui->statusBar->showMessage("Standby.");
            if(isAutoJump && isAutoJumpMode)
                on_pushButtonJump_clicked();
        }
        else if(getScreenshotMode <= 1)
        {
            isGetImage = false;
            ui->statusBar->showMessage("Switch get screenshot mode...");
            getScreenshotMode ++;
            on_pushButtonGetScreenshotImage_clicked();
        }
        else
        {
            isGetImage = false;
            ui->statusBar->showMessage("Get screenshot failed.");
        }
    }
}

void MainWindow::on_pushButtonJump_clicked()
{
    if(!isAdbInitializated)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Can't find adb.exe!"));
        msgBox.exec();
    }
    else
    {
        ui->statusBar->showMessage("Jumping...");
        QString cmd = adbFilePath + " shell input swipe 200 200 200 200 " +
                QString::number(jumpGame.getPressScreenTime());
        qDebug() << cmd;
        jumpProcess.start(cmd);
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
    initializeAdbServer();
}

void MainWindow::on_pushButtonGetScreenshotImage_clicked()
{
    if(!isAdbInitializated)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Adb initialization failed!"));
        msgBox.exec();
    }
    else
    {
        ui->statusBar->showMessage("Getting Screenshot...");
        QString cmd = adbFilePath + " shell screencap -p";
//        qDebug() << cmd;
        getScreenshotProcess.setProcessChannelMode(QProcess::MergedChannels);
        // it takes too much time
        getScreenshotProcess.start(cmd);
    }
}

void MainWindow::on_pushButtonRefreshAdb_clicked()
{
    initializeAdbServer();
}

void MainWindow::on_pushButtonTest_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("open image"),
                                                    "F:/1/ ",
                                                    tr("Image File(*.bmp *.jpg *.png"));
    if(filePath.isEmpty() | filePath.isNull())
        qDebug() << "filePath error!";
    else
        qImageScreenShot.load(filePath);
    if(qImageScreenShot.isNull())
        qDebug() << "image load fail!";
    else
    {
        matScreenShot = QImage2Mat_with_data(qImageScreenShot);
        jumpGame.setInputImage(matScreenShot);
        showImage(jumpGame.outputImage);
        ui->labelX1Y1->setText(QString::number(jumpGame.manLocationX())+" , "+QString::number(jumpGame.manLocationY()));
        ui->labelX2Y2->setText(QString::number(jumpGame.blockLocationX())+" , "+QString::number(jumpGame.blockLocationY()));
        ui->labelDistance->setText(QString::number(jumpGame.jumpDistance()));
        ui->labelParameter->setText(ui->lineEditDistanceParameter->text());
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
        ui->labelX1Y1->setText(QString::number(jumpGame.manLocationX())+" , "+QString::number(jumpGame.manLocationY()));
        ui->labelX2Y2->setText(QString::number(jumpGame.blockLocationX())+" , "+QString::number(jumpGame.blockLocationY()));
        ui->labelDistance->setText(QString::number(jumpGame.jumpDistance()));
        ui->labelParameter->setText(ui->lineEditDistanceParameter->text());
    }
}


void MainWindow::on_radioButtonManualJump_clicked()
{
    isAutoJumpMode = false;
    ui->pushButtonSwitchAutoJump->setEnabled(false);
    ui->pushButtonGetScreenshotImage->setEnabled(true);
    ui->pushButtonJump->setEnabled(true);
    if(isAutoJump)
        on_pushButtonSwitchAutoJump_clicked();
}

void MainWindow::on_radioButtonAutoJump_clicked()
{
    isAutoJumpMode = true;
    ui->pushButtonSwitchAutoJump->setEnabled(true);
    ui->pushButtonGetScreenshotImage->setEnabled(false);
    ui->pushButtonJump->setEnabled(false);
}

void MainWindow::on_pushButtonSwitchAutoJump_clicked()
{
    if(!isAutoJump)
    {
        ui->pushButtonSwitchAutoJump->setText("Stop(S)");
        ui->pushButtonSwitchAutoJump->setShortcut(Qt::Key_S);
        isAutoJump = true;
        connect(timerAuToJump,SIGNAL(timeout()),this,SLOT(timerAuToJumpTimeoutEvent()));
        connect(&jumpProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](){reloadAutoJumpTimer();});
        timerAuToJumpTimeoutEvent();
    }
    else
    {
        isAutoJump = false;
        if(getScreenshotProcess.state() == QProcess::Running || jumpProcess.state() == QProcess::Running)
            ui->pushButtonSwitchAutoJump->setEnabled(false);
        ui->pushButtonSwitchAutoJump->setText("Start(S)");
        ui->pushButtonSwitchAutoJump->setShortcut(Qt::Key_S);
        timerAuToJump->stop();
        disconnect(timerAuToJump,SIGNAL(timeout()),this,SLOT(timerAuToJumpTimeoutEvent()));
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
