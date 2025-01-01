#include "logwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QScrollBar>

LogWidget::LogWidget(QWidget *parent)
    : QWidget{parent}
{
    createInterface();
}

void LogWidget::createInterface()
{
    setFixedWidth(450);

    // Ajouter un layout avec un bouton comme contenu du widget
    QVBoxLayout *layout = new QVBoxLayout(this);

    d_logtext = new QTextEdit{this};
    d_logtext->setReadOnly(true);
    d_logtext->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d_logtext->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    d_logtext->setFixedHeight(300);

    QPalette palette = d_logtext->palette();
    palette.setColor(QPalette::Base, QColor("#0A0A0A")); // Couleur de fond
    d_logtext->setPalette(palette);

    layout->addWidget(d_logtext, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

bool LogWidget::addLog(const QString& text, int type)
{
    QString color = "#FFFFFF";


    switch (type) {
    case LogWidget::SUCCESS:
        color = "#34d399";
        break;
    case LogWidget::WARNING:
        color = "#fed7aa";
        break;
    case LogWidget::DANGER:
        color = "#F87171";
        break;
    }

    QString log = "<span style='margin-bottom:20px; color: " + color + ";'>" + text + "</span>";
    d_logtext->append(log);

    int pos = d_logtext->verticalScrollBar()->maximum();
    d_logtext->verticalScrollBar()->setValue(pos);
    return true;
}

void LogWidget::clearLog()
{
    d_logtext->clear();
}

LogWidget::~LogWidget()
{
    delete d_logtext;
}
