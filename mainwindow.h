#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "jumpjump.h"
#include "jumpcontroller.h"
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
    void receiveWidgetShowImageClickedPosInImage(int x,int y);
    void timerAuToJumpTimeoutEvent();
    void reloadAutoJumpTimer();

    void on_pushButtonJump_clicked();
    void on_pushButtonFindAdb_clicked();
    void on_pushButtonGetScreenshotImage_clicked();
    void on_pushButtonRefreshAdb_clicked();
    void on_pushButtonTest_clicked();
    void on_pushButtonLoadTemplate_clicked();
    void on_pushButtonUpdateProcessedImage_clicked();
    void on_radioButtonManualJump_clicked();
    void on_radioButtonAutoJump_clicked();
    void on_pushButtonSwitchAutoJump_clicked();
    void on_pushButtonTestSaveInputImage_clicked();
    void on_lineEditDistanceParameter_editingFinished();
    void receiveMatScreenshotAndProcess(cv::Mat img);
    void receiveJumpControllerMessage(QString msg);
private:
    void showImage(QImage &src);
    void showImage(cv::Mat &src);
    void getImageFromStdOutputAndProcessImage();
    void updateLables();

    int getScreenshotMode = 0;
    int getScreenshotModeErrorCount = 0;
    QTimer *timerAuToJump;
    int timerAuToJumpDelay = 1000;
    Ui::MainWindow *ui;
    QImage qImageScreenShot;
    cv::Mat matScreenShot;
    QImage qImageTemplate;
    cv::Mat matTemplate;
    JumpJump jumpGame;
    bool isGetImage = false;
    bool isAutoJumpMode = false;
    bool isAutoJump = false;

    QProcess adbProcess;
    QProcess jumpProcess;
    QProcess getScreenshotProcess;
    QString adbFilePath;
    bool isAdbInitializated = false;

    JumpController controller;
};

#endif // MAINWINDOW_H
