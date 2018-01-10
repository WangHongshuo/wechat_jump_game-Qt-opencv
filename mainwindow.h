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
    void showScreenshotImage(QImage &src);
    void showScreenshotImage(cv::Mat &src);
    void on_sliderCannyThreshold1_valueChanged(int value);
    void on_cannyThreshold2Slider_valueChanged(int value);

    void on_pushButtonJump_clicked();
    void on_pushButtonFindAdb_clicked();
    void on_pushButtonGetScreenshotImage_clicked();
    void on_pushButtonRefreshAdb_clicked();

    void on_pushButtonTest_clicked();

private:

    void showImage();
    void initializeAdbServer();
    Ui::MainWindow *ui;
    QImage qImageScreenShot;
    cv::Mat matScreenShot;
    JumpJump jumpGame;
    bool isGetImage = false;
    double distance = 0.0;

    double distanceParameter = 4.0;

    QProcess adbProcess;
    QString adbFilePath;
    bool isAdbInitializated = false;
    WCHAR *windowName = new WCHAR[100];
};

#endif // MAINWINDOW_H
