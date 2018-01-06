#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>
#include "jumpjump.h"
#include <QTimer>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void getMouseCursorPositionHWND();
    void on_checkBoxIsLock_stateChanged(int arg1);
    void on_pushButtonGetImage_clicked();
    void on_sliderCannyThreshold1_valueChanged(int value);
    void on_cannyThreshold2Slider_valueChanged(int value);
    void receiveDistance(double receiveData);
    void receivedX1Y1(QString receiveData);
    void receivedX2Y2(QString receiveData);
    void on_pushButtonJump_clicked();
    void receiveSelectedRectInfo(QByteArray in);
    void on_checkBoxAutoGetImage_stateChanged(int arg1);
    void on_pushButtonFindTargetWindow_clicked();
    void on_pushButtonStopFindTargetWindow_clicked();
    void on_pushButtonFindAdb_clicked();
    void on_pushButtonSetTempPath_clicked();
    void on_pushButtonGetScreenshotImage_clicked();

private:
    QString qStringWindowName;
    void showScreenshotImage(HWND window);
    void showScreenshotImage(QString tempImagePath);
    void showImage();
    HWND getMouseCursorPositionWindow();
    QString getCursorHWNDName(HWND window);
    void initializeAdbServer();
    Ui::MainWindow *ui;
    QScreen *screen = NULL;
    QImage screenShotQImage;
    cv::Mat screenShotMat;
    HWND targetWindow;
    JumpJump jumpGame;
    bool isGetImage = false;
    double distance = 0.0;
    QTimer *timerGetNextFrame;
    QTimer *timerGetTargetWindow;
    double distanceParameter = 4.0;
    POINT currentMousePostion;
    QProcess adbProcess;
    QString adbFilePath;
    QString tempImageSavePath;
    bool isAdbInitializated = false;
    WCHAR *windowName = new WCHAR[100];
};

#endif // MAINWINDOW_H
