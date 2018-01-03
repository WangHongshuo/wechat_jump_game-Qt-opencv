#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>

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

private:
    QString qStringWindowName;
    void showScreenshotImage(HWND window);
    HWND getTargetWindow();
    Ui::MainWindow *ui;
    QScreen *screen = NULL;
    QImage *screenShotQImage = new QImage;
    HWND targetWindow;
};

#endif // MAINWINDOW_H
