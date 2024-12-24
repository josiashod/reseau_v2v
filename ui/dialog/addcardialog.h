#ifndef ADDCARDIALOG_H
#define ADDCARDIALOG_H

#include <QDialog>

class QLineEdit;
class QDial;
class QLabel;

class AddCarDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddCarDialog(QWidget *parent = nullptr);

signals:
    // signal the nomber of car to be created with a chosen freq and power
    void create_car(int nb, double speed, double freq, double intensity);

private slots:
    void onAdd();

private:
    void create_interface();
    QLineEdit* d_nm_car_edit;
    QDial* d_speed_dial;
    QDial* d_freq_dial;
    QDial* d_intensity_dial;
    QLabel* d_speed_label;
    QLabel* d_freq_label;
    QLabel* d_intensity_label;
};

#endif // ADDCARDIALOG_H
