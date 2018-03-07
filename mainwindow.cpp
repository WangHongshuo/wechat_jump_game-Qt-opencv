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
    // ֻ��������
    QRegExp rx("^(-?[0]|-?[1-9][0-9]{0,5})(?:\\.\\d{1,4})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    adbFilePath = QCoreApplication::applicationDirPath() + "\\adb\\adb";
    ui->lineEditDistanceParameter->setValidator(pReg);
    ui->lineEditDistanceParameter->setText(QString::number(distanceParameter));

    ui->widgetShowImage->setEnableDragImage(false);
    ui->widgetShowImage->setEnableZoomImage(false);
    ui->widgetShowImage->setEnableRecordLastParameters(true);
    ui->widgetShowTemplate->setEnableOnlyShowImage(true);
    ui->widgetShowImage->setEnableSendLeftClickedPosInImage(true);
    connect(ui->widgetShowImage,SIGNAL(sendLeftClickedPosInImage(int,int)),this,SLOT(receiveWidgetShowImageClickedPosInImage(int,int)));
    initializeAdbServer();

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
        ui->lineEditAdbState->setText("adb error!");
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
                ui->lineEditAdbState->setText("No Devices!");
                isAdbInitializated = false;
            }
            else
            {
                ui->lineEditAdbState->setText("Find: " + output.mid(25));
                isAdbInitializated = true;
            }
        }
        else
        {
            ui->lineEditAdbState->setText("adb error!");
            isAdbInitializated = false;
        }
    }
}

void MainWindow::on_sliderCannyThreshold1_valueChanged(int value)
{
    if(jumpGame.isLoadInputImage())
    {
        jumpGame.cannyThreshold1 = value;
        jumpGame.update();
        showImage(jumpGame.edgeImage);
    }
}

void MainWindow::on_cannyThreshold2Slider_valueChanged(int value)
{
    if(jumpGame.isLoadInputImage())
    {
        jumpGame.cannyThreshold2 = value;
        jumpGame.update();
        showImage(jumpGame.edgeImage);
    }
}

void MainWindow::receiveWidgetShowImageClickedPosInImage(int x, int y)
{
    jumpGame.setLeftClickedPos(x,y);
    showImage(jumpGame.outputImage);
    ui->labelX1Y1->setText(QString::number(jumpGame.manLocationX())+" , "+QString::number(jumpGame.manLocationY()));
    ui->labelX2Y2->setText(QString::number(x)+" , "+QString::number(y));
    ui->labelDistance->setText(QString::number(jumpGame.distance));
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
        distanceParameter = ui->lineEditDistanceParameter->text().toDouble();
        QString cmd = adbFilePath + " shell input swipe 200 200 200 200 " +
                QString::number(int(distanceParameter*jumpGame.distance));
        qDebug() << cmd;
        adbProcess.start(cmd);
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
        QString cmd = adbFilePath + " shell screencap -p";
//        qDebug() << cmd;
        adbProcess.setProcessChannelMode(QProcess::MergedChannels);
        // it takes too much time
        adbProcess.start(cmd);
        if(!adbProcess.waitForFinished())
            qDebug() << "Error or timeout!";
        QByteArray data;
        data = adbProcess.readAll();
//        qDebug() << data.length();
        data = data.replace("\r\r\n","\n");
//        qDebug() << data.length();

        // to Mat
        std::vector<uchar> buffer(data.begin(),data.end());
        matScreenShot = cv::imdecode(buffer,CV_LOAD_IMAGE_COLOR);

        // to QImage
//        QBuffer buffer(&data);
//        QImageReader reader(&buffer);
//        reader.setFormat("PNG");
//        qImageScreenShot = reader.read();

        if(matScreenShot.data )
        {
            isGetImage = true;
            qDebug() << "1";
            jumpGame.setInputImage(matScreenShot);
            showImage(jumpGame.outputImage);
        }
        else
            isGetImage = false;
    }
}

void MainWindow::on_pushButtonRefreshAdb_clicked()
{
    initializeAdbServer();
}

void MainWindow::on_pushButtonTest_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("open image"),
                                                    " ",
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
