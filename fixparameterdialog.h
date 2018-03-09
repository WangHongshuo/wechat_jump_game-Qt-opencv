#ifndef FIXPARAMETERDIALOG_H
#define FIXPARAMETERDIALOG_H

#include <QDialog>

namespace Ui {
class FixParameterDialog;
}

class FixParameterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FixParameterDialog(QWidget *parent = 0);
    ~FixParameterDialog();
signals:
    void sendAdjustedCorrectionValue(double delta,int index);
private slots:
    void on_comboBoxStep_currentIndexChanged(int index);

    void on_pushButtonInc_clicked();

    void on_pushButtonDec_clicked();

private:
    Ui::FixParameterDialog *ui;
    void initializedTableViewParameterInfo();

    double steps[3] = {0.001,0.01,0.1};
    double step = 0.001;
};

#endif // FIXPARAMETERDIALOG_H
