#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>
#include "jumpjump.h"
#include <QTimer>

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
    void on_findTargetWindowPushButton_clicked();

    void on_getTargetHWNDPushButton_clicked();

    void on_isLockCheckBox_stateChanged(int arg1);

    void on_getImagePushButton_clicked();

    void on_cannyThreshold1Slider_valueChanged(int value);

    void on_cannyThreshold2Slider_valueChanged(int value);

    void receiveDistance(double receiveData);
    void receivedX1Y1(QString receiveData);
    void receivedX2Y2(QString receiveData);
    void on_jumpPushButton_clicked();

    void on_autoGetImageCheckBox_stateChanged(int arg1);

private:
    QString qStringWindowName;
    void showScreenshotImage(HWND window);
    void showImage();
    HWND getTargetWindow();
    Ui::MainWindow *ui;
    QScreen *screen = NULL;
    QImage screenShotQImage;
    cv::Mat screenShotMat;
    HWND targetWindow;
    JumpJump jumpGame;
    bool isGetImage = false;
    double distance = 0.0;
    QTimer *autoTimer;
    double distanceParameter = 5.0;
    POINT currentMousePostion;
};

#endif // MAINWINDOW_H
