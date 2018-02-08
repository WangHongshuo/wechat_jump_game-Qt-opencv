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
    void showImage(QImage &src);
    void showImage(cv::Mat &src);
    void on_sliderCannyThreshold1_valueChanged(int value);
    void receiveWidgetShowImageClickedPosInImage(int x,int y);
    void timerAuToJumpTimeoutEvent();
    void on_pushButtonJump_clicked();
    void on_pushButtonFindAdb_clicked();
    void on_pushButtonGetScreenshotImage_clicked();
    void on_pushButtonRefreshAdb_clicked();
    void on_pushButtonTest_clicked();
    void on_pushButtonLoadTemplate_clicked();
    void on_checkBoxEnableFixCannyT_stateChanged(int arg1);
    void on_sliderCannyThreshold2_valueChanged(int value);
    void on_pushButtonUpdateProcessedImage_clicked();
    void on_radioButtonManualJump_clicked();
    void on_radioButtonAutoJump_clicked();
    void on_pushButtonSwitchAutoJump_clicked();

    void on_pushButtonTestSaveInputImage_clicked();

private:
    void showImage();
    void initializeAdbServer();


    QTimer *timerAuToJump;
    int timerAuToJumpInterval = 0;
    int timerAuToJumpDelay = 2500;
    Ui::MainWindow *ui;
    QImage qImageScreenShot;
    cv::Mat matScreenShot;
    QImage qImageTemplate;
    cv::Mat matTemplate;
    JumpJump jumpGame;
    bool isGetImage = false;
    bool isAutoJump = false;
    double distanceParameter = 1.45;

    QProcess adbProcess;
    QString adbFilePath;
    bool isAdbInitializated = false;
};

#endif // MAINWINDOW_H
