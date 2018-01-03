#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScreen>
#include <TlHelp32.h>
#include <windows.h>
#include <QDebug>
#include <QMessageBox>
#include <QWindow>
#include <QTimer>
#include "mat_qimage_convert.h"

// UTF-8 without BOM

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QRegExp rx("^(-?[0]|-?[1-9][0-9]{0,5})(?:\\.\\d{1,4})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->distanceParameterLineEdit->setValidator(pReg);
    ui->distanceParameterLineEdit->setText(QString::number(distanceParameter));
    autoTimer = new QTimer(this);
    ui->getTargetHWNDPushButton->setEnabled(false);
    ui->showImageWidget->set_enable_drag_image(false);
    ui->showImageWidget->set_enable_zoom_image(false);
    connect(ui->showImageWidget,SIGNAL(send_distance(double)),this,SLOT(receiveDistance(double)));
    connect(ui->showImageWidget,SIGNAL(send_x1_y1(QString)),this,SLOT(receivedX1Y1(QString)));
    connect(ui->showImageWidget,SIGNAL(send_x2_y2(QString)),this,SLOT(receivedX2Y2(QString)));
    connect(this->autoTimer,SIGNAL(timeout()),this,SLOT(on_getImagePushButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_findTargetWindowPushButton_clicked()
{
    targetWindow = getTargetWindow();
    showScreenshotImage(targetWindow);
}

void MainWindow::showScreenshotImage(HWND window)
{ 
    screen = QGuiApplication::primaryScreen();
    screenShotQImage = (screen->grabWindow((WId)window)).toImage();


    // To Do Something
    screenShotMat = QImage2Mat_with_pointer(screenShotQImage);
    int w = screenShotMat.cols;
    int h = screenShotMat.rows;
    screenShotMat = screenShotMat(cv::Rect(0,(int)(h*0.3),w,(int)(h*(1-0.5))));
    jumpGame.setImage(screenShotMat);
    isGetImage = true;
    screenShotQImage = Mat2QImage_with_pointer(jumpGame.outputImage);
    ui->showImageWidget->set_image_with_pointer(&screenShotQImage);
    w = screenShotMat.cols;
    h = screenShotMat.rows;
}

void MainWindow::showImage()
{
    screenShotQImage = Mat2QImage_with_pointer(jumpGame.outputImage);
    ui->showImageWidget->set_image_with_pointer(&screenShotQImage);
}

HWND MainWindow::getTargetWindow()
{
    // 寻找匹配窗口名的窗口
    qStringWindowName = ui->getWindowNameLineEdit->text();
    LPCWSTR windowName = reinterpret_cast<LPCWSTR>(qStringWindowName.data());
    HWND window = FindWindowW(NULL,windowName);
    if(window == 0)
    {
        QMessageBox msgBox(QMessageBox::Critical,tr("错误"),tr("未找到窗口!"));
        msgBox.exec();
        return window;
    }
    else
    {
        QString temp = "找到窗口 : " + qStringWindowName;
        QMessageBox msgBox(QMessageBox::Information,tr("提示"),(temp));
        msgBox.exec();
        ui->getTargetHWNDPushButton->setEnabled(true);
        return window;
    }
}

void MainWindow::on_getTargetHWNDPushButton_clicked()
{
    // 寻找该窗口下的子窗口，获取最好效果
    targetWindow = GetWindow(targetWindow,GW_CHILD);
    showScreenshotImage(targetWindow);
}

void MainWindow::on_isLockCheckBox_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->getTargetHWNDPushButton->setEnabled(false);
        ui->getWindowNameLineEdit->setEnabled(false);
        ui->findTargetWindowPushButton->setEnabled(false);
    }
    else
    {
        ui->getWindowNameLineEdit->setEnabled(true);
        ui->findTargetWindowPushButton->setEnabled(true);
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
    RECT windowRect;
    ::GetWindowRect(targetWindow,&windowRect);
    ::GetCursorPos(&currentMousePostion);
    ::SetCursorPos(windowRect.left+20,windowRect.top+100);
    ::Sleep(100);
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
    distanceParameter = ui->distanceParameterLineEdit->text().toDouble();

    ::Sleep(int(distance*distanceParameter));

    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
    ::SetCursorPos(currentMousePostion.x,currentMousePostion.y);
}


void MainWindow::on_autoGetImageCheckBox_stateChanged(int arg1)
{
    if(arg1)
    {
        autoTimer->setInterval(1000/24);
        autoTimer->start();
    }
    else
        autoTimer->stop();
}
