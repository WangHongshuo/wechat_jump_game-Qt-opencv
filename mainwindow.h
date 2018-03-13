#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fixparameterdialog.h"
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

signals:
    void sendCurrentJumpLog(double *log);
public slots:
    void receiveAdjustedParameterValue(double delta,int index);
private slots:
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

    void receiveWidgetShowImageClickedPosInImage(int x,int y);
    void receiveMatScreenshotAndProcess(cv::Mat img);
    void receiveJumpControllerMessage(QString msg);
    void receiveSaveCorrectionsAction(QString path);

    void on_pushButtonLoadIniFile_clicked();

    void on_pushButtonFixPatameters_clicked();
    void setPushButtonFixParametersEnable();

private:
    void showImage(QImage &src);
    void showImage(cv::Mat &src);
    void updateLables();
    void showAdbServiceInitFailedMsgBox();
    void showNoDeviceDetectedMsgBox();

    Ui::MainWindow *ui;
    QImage qImageScreenShot;
    cv::Mat matScreenShot;
    QImage qImageTemplate;
    cv::Mat matTemplate;

    JumpJump jumpGame;
    bool isAutoJumpModeSelected = false;
    bool isAutoJumpActived = false;

    QString adbFilePath;

    JumpController controller;
};

#endif // MAINWINDOW_H
