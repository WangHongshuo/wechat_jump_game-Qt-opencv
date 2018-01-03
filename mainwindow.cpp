#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScreen>
#include <TlHelp32.h>
#include <windows.h>
#include <QDebug>
#include <QMessageBox>

// UTF-8 without BOM

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->getTargetHWNDPushButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete screenShotQImage;
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
    *screenShotQImage = (screen->grabWindow((WId)window)).toImage();
    // To Do Something

    ui->showImageWidget->set_image_with_pointer(screenShotQImage);
}

HWND MainWindow::getTargetWindow()
{
    // 寻找匹配窗口名的窗口
    qStringWindowName = ui->getWindowNameLineEdit->text();
    LPCWSTR windowName = reinterpret_cast<LPCWSTR>(qStringWindowName.data());
    HWND window = FindWindowW(NULL,windowName);
    qDebug() << window;
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
