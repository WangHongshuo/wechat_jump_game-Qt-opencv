#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScreen>
#include <TlHelp32.h>
#include <windows.h>
#include <QDebug>
#include <QMessageBox>
#include <QWindow>
#include <QTimer>
#include <iostream>
#include "mat_qimage_convert.h"

// ANSI

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // ֻ��������
    QRegExp rx("^(-?[0]|-?[1-9][0-9]{0,5})(?:\\.\\d{1,4})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->distanceParameterLineEdit->setValidator(pReg);
    ui->distanceParameterLineEdit->setText(QString::number(distanceParameter));

    timerGetNextFrame = new QTimer(this);
    timerGetTargetWindow = new QTimer(this);
    ui->showImageWidget->set_enable_drag_image(false);
    ui->showImageWidget->set_enable_zoom_image(false);
    ui->stopFindTargetWindowPushButton->setEnabled(false);

    connect(ui->showImageWidget,SIGNAL(send_distance(double)),this,SLOT(receiveDistance(double)));
    connect(ui->showImageWidget,SIGNAL(send_x1_y1(QString)),this,SLOT(receivedX1Y1(QString)));
    connect(ui->showImageWidget,SIGNAL(send_x2_y2(QString)),this,SLOT(receivedX2Y2(QString)));
    connect(this->timerGetNextFrame,SIGNAL(timeout()),this,SLOT(on_getImagePushButton_clicked()));
    connect(this->timerGetTargetWindow,SIGNAL(timeout()),this,SLOT(getMouseCursorPositionHWND()));
    connect(ui->showImageWidget,SIGNAL(send_selected_rect_info(QByteArray)),this,SLOT(receiveSelectedRectInfo(QByteArray)));
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
    ui->showWindowNameLineEdit->setText(getCursorHWNDInfo(targetWindow));
    ui->showWindowNameLineEdit->setCursorPosition(1);
    qDebug() << targetWindow;
    ui->showWindowHandleLineEdit->setText(QString::number((long)targetWindow,16));
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

QString MainWindow::getCursorHWNDInfo(HWND window)
{
    ::GetWindowTextW(window,windowName,99);
    QString qWindowName = QString::fromWCharArray(windowName);
    return qWindowName;
}

void MainWindow::on_isLockCheckBox_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->showWindowNameLineEdit->setEnabled(false);
    }
    else
    {
        ui->showWindowNameLineEdit->setEnabled(true);
    }
}

void MainWindow::on_getImagePushButton_clicked()
{
    showScreenshotImage(targetWindow);
}

void MainWindow::on_cannyThreshold1Slider_valueChanged(int value)
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
    ui->distanceLabel->setText(QString::number(distance));
}

void MainWindow::receivedX1Y1(QString receiveData)
{
    ui->x1y1Label->setText(receiveData);
}

void MainWindow::receivedX2Y2(QString receiveData)
{
    ui->x2y2Label->setText(receiveData);
}

void MainWindow::on_jumpPushButton_clicked()
{
    if(!isAdbInitializated)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Can't find adb.exe!"));
        msgBox.exec();
    }
    else
    {
        distanceParameter = ui->distanceParameterLineEdit->text().toDouble();
        QString cmd = filePath + " shell input swipe 200 200 200 200 " +
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


void MainWindow::on_autoGetImageCheckBox_stateChanged(int arg1)
{
    if(arg1)
    {
        timerGetNextFrame->setInterval(1000/24);
        timerGetNextFrame->start();
    }
    else
        timerGetNextFrame->stop();
}

void MainWindow::on_findTargetWindowPushButton_clicked()
{
    ui->findTargetWindowPushButton->setEnabled(false);
    ui->stopFindTargetWindowPushButton->setEnabled(true);
    timerGetTargetWindow->setInterval(500);
    timerGetTargetWindow->start();
}

void MainWindow::on_stopFindTargetWindowPushButton_clicked()
{
    timerGetTargetWindow->stop();
    ui->findTargetWindowPushButton->setEnabled(true);
    ui->stopFindTargetWindowPushButton->setEnabled(false);
}

void MainWindow::on_findAdbpushButton_clicked()
{
    filePath = QFileDialog::getOpenFileName(this,tr("Open adb.exe"),
                                                        "C:/",
                                                        tr("adb file(adb.exe)"));
    filePath.chop(4);
    adbProcess.start(filePath);
    if(!adbProcess.waitForFinished())
    {
        ui->adbStateNameLineEdit->setText("adb error!");
        isAdbInitializated = false;
    }
    else
    {
        QString output =QString::fromLocal8Bit(adbProcess.readAllStandardError());
        if(output.startsWith("Android Debug Bridge"))
        {
            ui->adbStateNameLineEdit->setText("adb ok!");
            isAdbInitializated = true;
        }
        else
        {
            ui->adbStateNameLineEdit->setText("adb error!");
            isAdbInitializated = false;
        }
    }
}
