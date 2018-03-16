#include "jumpcontroller.h"
#include <QString>
#include <QObject>
#include <QDebug>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <QTime>
#include <QtGlobal>

JumpController::JumpController()
{
    getScreenshotProcess.setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect(&getScreenshotProcess,SIGNAL(finished(int)),
                     this,SLOT(getImageFromStdOutput()));
    QObject::connect(&jumpProcess,SIGNAL(finished(int)),
                     this,SLOT(jumpActionFinishedEvent()));
}

bool JumpController::initializeAdbService(QString path)
{
    adbFilePath = path;
    return initialize();
}

bool JumpController::refreshAdbService(QString path)
{
    return initializeAdbService(path);
}

bool JumpController::refreshAdbService()
{
    if(isAdbServiceInitializated)
    {
        return initialize();
    }
    else
    {
        emit sendJumpControllerMessage(QString("Adb path is invalid!"));
        return false;
    }
}

bool JumpController::restartAdbService(QString path)
{
    killAdbService(path);
    return initializeAdbService(path);
}

bool JumpController::restartAdbService()
{
    if(isAdbPathValid)
    {
        killAdbService(adbFilePath);
        return initialize();
    }
    else
    {
        emit sendJumpControllerMessage(QString("Adb path is invalid!"));
        return false;
    }
}

void JumpController::startAutoJumpLoop()
{
    if(isAdbPathValid && isAdbServiceInitializated && isDetectedDevice)
    {
        QObject::connect(&jumpProcess,SIGNAL(finished(int)),
                         this,SLOT(jumpActionFinishedLoopEvent()));
        QObject::connect(&timerJumpInterval,SIGNAL(timeout()),
                         this,SLOT(timerJumpIntervalTimeoutEvent()));
        getMatScreenshotImage();
    }
    else
    {
        sendJumpControllerMessage(QString("Adb error or no device!"));
    }
}

void JumpController::stopAutoJumpLoop()
{
    timerJumpInterval.stop();
    if(jumpProcess.state() == QProcess::Running)
        jumpProcess.terminate();
    if(getScreenshotProcess.state() == QProcess::Running)
        getScreenshotProcess.terminate();
    QObject::disconnect(&jumpProcess,SIGNAL(finished(int)),
                        this,SLOT(jumpActionFinishedLoopEvent()));
    QObject::disconnect(&timerJumpInterval,SIGNAL(timeout()),
                        this,SLOT(timerJumpIntervalTimeoutEvent()));
}

void JumpController::getMatScreenshotImage()
{
    if(isAdbServiceInitializated && isDetectedDevice)
    {
        emit sendJumpControllerMessage(QString("Getting Screenshot..."));
        cmd = adbFilePath + " shell screencap -p";
        // it takes too much time
        getScreenshotProcess.start(cmd);
    }
}

void JumpController::setCheatMode(bool flag)
{
    isCheatMode = flag;
}

bool JumpController::isAdbServiceInitializatedFlag() const
{
    return isAdbServiceInitializated;
}

bool JumpController::isDetectedDeviceFlag() const
{
    return isDetectedDevice;
}

bool JumpController::initialize()
{
    adbProcess.start(adbFilePath + " devices");
    adbProcess.waitForFinished();
    adbProcess.start(adbFilePath + " devices");
    if(!adbProcess.waitForFinished())
    {
        emit sendJumpControllerMessage(QString("Adb error!"));
        isAdbServiceInitializated = false;
        return false;
    }
    else
    {
        QString output =QString::fromLocal8Bit(adbProcess.readAllStandardOutput());
        output = output.simplified();
        if(output.startsWith("List of devices attached"))
        {
            isAdbPathValid = true;
            isAdbServiceInitializated = true;
            if(output.mid(25).isEmpty() || output.mid(25).isNull())
            {
                emit sendJumpControllerMessage(QString("No Devices!"));
                isDetectedDevice = false;
            }
            else
            {
                emit sendJumpControllerMessage(QString("Find: " + output.mid(25)));
                isDetectedDevice = true;
            }
            return true;
        }
        else
        {
            emit sendJumpControllerMessage(QString("Adb error!"));
            isAdbPathValid = false;
            isAdbServiceInitializated = false;
            return false;
        }
    }
}

void JumpController::jumpAction(int pressTime)
{
    if(pressTime > 0)
    {
        if(!isCheatMode)
            cmd = adbFilePath + " shell input swipe 200 200 200 200 " +
                QString::number(pressTime);
        else
        {
            qsrand(QTime::currentTime().second());
            int x,y;
            x = 250+qrand()%100;
            y = 1600+qrand()%100;
            cmd = adbFilePath + " shell input swipe "+
                    QString::number(x)+" "+
                    QString::number(y)+" "+
                    QString::number(x)+" "+
                    QString::number(y)+" "+
                    QString::number(pressTime);
        }
        qDebug() << cmd;
        jumpProcess.start(cmd);
        emit sendJumpControllerMessage(QString("Jumpping..."));
    }
    else
        emit sendJumpControllerMessage(QString("Jump Time invalid!"));
}

void JumpController::killAdbService(QString path)
{
    adbProcess.start(path + "kill-server");
    adbProcess.waitForFinished();
}

void JumpController::killAdbService()
{
    if(isAdbPathValid)
    {
        adbProcess.start(adbFilePath + "kill-server");
        adbProcess.waitForFinished();
        emit sendJumpControllerMessage(QString("Kill adb service successful!"));
    }
    else
        emit sendJumpControllerMessage(QString("Adb path is invalid!"));
}

void JumpController::getImageFromStdOutput()
{
    QByteArray data;
    data = getScreenshotProcess.readAll();
    getScreenshotProcess.terminate();
    data = data.replace("\r\r\n","\n");

    // to Mat
    std::vector<uchar> buffer(data.begin(),data.end());
    matScreenshot = cv::imdecode(buffer,CV_LOAD_IMAGE_COLOR);
    if(matScreenshot.data != NULL)
    {
        emit sendMatScreenshotImage(matScreenshot);
        emit sendJumpControllerMessage(QString("Standby."));
    }
}

void JumpController::jumpActionFinishedEvent()
{
    emit sendJumpControllerMessage(QString("Standby."));
}

void JumpController::jumpActionFinishedLoopEvent()
{
    timerJumpInterval.setInterval(jumpInterval);
    timerJumpInterval.start();
    emit sendJumpControllerMessage(QString("Wait for jump finished."));
}

void JumpController::timerJumpIntervalTimeoutEvent()
{
    timerJumpInterval.stop();
    getMatScreenshotImage();
}
