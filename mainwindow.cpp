#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScreen>
#include <TlHelp32.h>
#include <windows.h>
#include <QDebug>
#include <QMessageBox>
#include <QWindow>
#include <QTimer>
#include <QTime>
#include <iostream>
#include "mat_qimage_convert.h"

// ANSI

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 只接受数字
    QRegExp rx("^(-?[0]|-?[1-9][0-9]{0,5})(?:\\.\\d{1,4})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    tempImageSavePath = QCoreApplication::applicationDirPath();
    adbFilePath = QCoreApplication::applicationDirPath() + "\\adb\\adb";
    ui->lineEditTempPath->setText(tempImageSavePath + "/temp.png");
    ui->lineEditTempPath->setCursorPosition(1);
    ui->lineEditDistanceParameter->setValidator(pReg);
    ui->lineEditDistanceParameter->setText(QString::number(distanceParameter));

    timerGetNextFrame = new QTimer(this);
    timerGetTargetWindow = new QTimer(this);
    ui->showImageWidget->set_enable_drag_image(false);
    ui->showImageWidget->set_enable_zoom_image(false);
    ui->pushButtonStopFindTargetWindow->setEnabled(false);

    connect(ui->showImageWidget,SIGNAL(send_distance(double)),this,SLOT(receiveDistance(double)));
    connect(ui->showImageWidget,SIGNAL(send_x1_y1(QString)),this,SLOT(receivedX1Y1(QString)));
    connect(ui->showImageWidget,SIGNAL(send_x2_y2(QString)),this,SLOT(receivedX2Y2(QString)));
    connect(this->timerGetNextFrame,SIGNAL(timeout()),this,SLOT(on_pushButtonGetImage_clicked()));
    connect(this->timerGetTargetWindow,SIGNAL(timeout()),this,SLOT(getMouseCursorPositionHWND()));
    connect(ui->showImageWidget,SIGNAL(send_selected_rect_info(QByteArray)),this,SLOT(receiveSelectedRectInfo(QByteArray)));
    initializeAdbServer();
}

MainWindow::~MainWindow()
{
    delete windowName;
    delete ui;
}

void MainWindow::getMouseCursorPositionHWND()
{
    targetWindow = getMouseCursorPositionWindow();
    showScreenshotImage(targetWindow);
    ui->lineEditShowWindowName->setText(getCursorHWNDName(targetWindow));
    ui->lineEditShowWindowName->setCursorPosition(1);
//    qDebug() << targetWindow;
    ui->lineEditShowWindowHandle->setText(QString::number((long)targetWindow,16));
}

void MainWindow::showScreenshotImage(HWND window)
{ 
    screen = QGuiApplication::primaryScreen();
    screenShotQImage = (screen->grabWindow((WId)window)).toImage();

    // To Do Something
    screenShotMat = QImage2Mat_with_pointer(screenShotQImage);
//    int w = screenShotMat.cols;
//    int h = screenShotMat.rows;
//    screenShotMat = screenShotMat(cv::Rect(0,(int)(h*0.3),w,(int)(h*(1-0.5))));
    jumpGame.setImage(screenShotMat);
    isGetImage = true;
    screenShotQImage = Mat2QImage_with_pointer(jumpGame.outputImage);

    ui->showImageWidget->set_image_with_pointer(&screenShotQImage);
}

void MainWindow::showScreenshotImage(QString tempImagePath)
{
    screenShotQImage.load(tempImagePath);
    screenShotQImage = screenShotQImage.convertToFormat(QImage::Format_RGB888);
    qDebug() << screenShotQImage.bits();
    screenShotMat = QImage2Mat_with_pointer(screenShotQImage);
    qDebug() << screenShotMat.data;
    // To do something


    screenShotQImage = Mat2QImage_with_pointer(screenShotMat);
    qDebug() << screenShotQImage.bits();
//    ui->showImageWidget->set_image_with_pointer(&screenShotQImage);
}

void MainWindow::showImage()
{
    screenShotQImage = Mat2QImage_with_pointer(jumpGame.outputImage);
    ui->showImageWidget->set_image_with_pointer(&screenShotQImage);
}

HWND MainWindow::getMouseCursorPositionWindow()
{
    ::GetCursorPos(&currentMousePostion);
    HWND window = WindowFromPoint(currentMousePostion);
    return window;
}

QString MainWindow::getCursorHWNDName(HWND window)
{
    ::GetWindowTextW(window,windowName,99);
    QString qWindowName = QString::fromWCharArray(windowName);
    return qWindowName;
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
                ui->lineEditAdbState->setText("No Devices!");
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

void MainWindow::on_checkBoxIsLock_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->lineEditShowWindowName->setEnabled(false);
    }
    else
    {
        ui->lineEditShowWindowName->setEnabled(true);
    }
}

void MainWindow::on_pushButtonGetImage_clicked()
{
    showScreenshotImage(targetWindow);
}

void MainWindow::on_sliderCannyThreshold1_valueChanged(int value)
{
    if(isGetImage)
    {
        jumpGame.cannyThreshold1 = value;
        jumpGame.update();
        showImage();
    }
}

void MainWindow::on_cannyThreshold2Slider_valueChanged(int value)
{
    if(isGetImage)
    {
        jumpGame.cannyThreshold2 = value;
        jumpGame.update();
        showImage();
    }
}

void MainWindow::receiveDistance(double receiveData)
{
    distance = receiveData;
    ui->labelDistance->setText(QString::number(distance));
}

void MainWindow::receivedX1Y1(QString receiveData)
{
    ui->labelX1Y1->setText(receiveData);
}

void MainWindow::receivedX2Y2(QString receiveData)
{
    ui->labelX2Y2->setText(receiveData);
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
                QString::number(int(distanceParameter*distance));
//        qDebug() << cmd;
        adbProcess.start(cmd);
    }
}

void MainWindow::receiveSelectedRectInfo(QByteArray in)
{
    quint8 x, y, w, h;
    QDataStream inStream(&in,QIODevice::ReadOnly);
    inStream.setVersion(QDataStream::Qt_5_10);
    inStream >> x >> y >> w >> h;
//    qDebug() << "rece: " << x << y << w << h;
}


void MainWindow::on_checkBoxAutoGetImage_stateChanged(int arg1)
{
    if(arg1)
    {
        timerGetNextFrame->setInterval(1000/24);
        timerGetNextFrame->start();
    }
    else
        timerGetNextFrame->stop();
}

void MainWindow::on_pushButtonFindTargetWindow_clicked()
{
    ui->pushButtonFindTargetWindow->setEnabled(false);
    ui->pushButtonStopFindTargetWindow->setEnabled(true);
    timerGetTargetWindow->setInterval(500);
    timerGetTargetWindow->start();
}

void MainWindow::on_pushButtonStopFindTargetWindow_clicked()
{
    timerGetTargetWindow->stop();
    ui->pushButtonFindTargetWindow->setEnabled(true);
    ui->pushButtonStopFindTargetWindow->setEnabled(false);
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

void MainWindow::on_pushButtonSetTempPath_clicked()
{
    tempImageSavePath = QFileDialog::getExistingDirectory(this,tr("Where to set temp image"),
                                                          QCoreApplication::applicationDirPath());
    if(tempImageSavePath.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Path is empty!"));
        msgBox.exec();
        tempImageSavePath = QCoreApplication::applicationDirPath();
    }
    ui->lineEditTempPath->setText(tempImageSavePath + "/temp.bmp");
    ui->lineEditTempPath->setCursorPosition(1);
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
        QString path = tempImageSavePath.replace(QString("/"),QString("\\"));
        QString cmd = adbFilePath + " shell /system/bin/screencap -p /sdcard/temp.png";
//        qDebug() << cmd;

        // it takes too much time
        adbProcess.start(cmd);
        if(!adbProcess.waitForFinished())
            ui->lineEditAdbState->setText("Error in get Screenshot!");
//        qDebug() << QString::fromLocal8Bit(adbProcess.readAllStandardOutput());
        else
        {
            cmd = adbFilePath + " pull /sdcard/temp.png " + path;
//            qDebug() << cmd;
            adbProcess.start(cmd);
            if(adbProcess.waitForFinished())
            {
                ui->lineEditAdbState->setText("Done!");
                showScreenshotImage(tempImageSavePath + "\\temp.png");
            }
            else
                ui->lineEditAdbState->setText("Error in copying image to disk.");
//            qDebug() << QString::fromLocal8Bit(adbProcess.readAllStandardOutput());
        }
    }
}
