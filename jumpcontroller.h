#ifndef JUMPCONTROLLER_H
#define JUMPCONTROLLER_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QImage>
#include <QTimer>
#include "jumpjump.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

class JumpController : public QObject
{
    Q_OBJECT

public:
    JumpController();
    bool initializeAdbService(QString path);
    void killAdbService(QString path);
    void killAdbService();
    bool refreshAdbService(QString path);
    bool refreshAdbService();
    bool restartAdbService(QString path);
    bool restartAdbService();
    void startAutoJumpLoop();
    void stopAutoJumpLoop();
    void getMatScreenshotImage();
    QImage getQImageScreenshotImage();

    bool isAdbServiceInitializatedFlag() const;
    bool isDetectedDeviceFlag() const;


signals:
    void sendJumpControllerMessage(QString msg);
    void sendMatScreenshotImage(cv::Mat img);
    void sendAllProcessTerminated();
public slots:
    void jumpAction(int pressTime);
private slots:
    void getImageFromStdOutput();
    void jumpActionFinishedEvent();
    void jumpActionFinishedLoopEvent();
    void timerJumpIntervalTimeoutEvent();
private:
    bool initialize();

    QTimer timerJumpInterval;
    int jumpInterval = 1000;
    JumpJump imageProcesser;
    QProcess adbProcess;
    QProcess getScreenshotProcess;
    QProcess jumpProcess;
    QString adbFilePath;
    cv::Mat matScreenshot;
    QImage qImageScreenshot;
    bool isAdbPathValid = false;
    bool isAdbServiceInitializated = false;
    bool isDetectedDevice = false;
};

#endif // JUMPCONTROLLER_H
