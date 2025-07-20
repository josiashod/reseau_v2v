#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class LogWidget;
class QLineEdit;

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);

public slots:
    void onLoadFile();
    void onLoadDB();

private:
    LogWidget* d_logsWidget;
    QLineEdit* d_edit;

};

#endif // WIDGET_H
