#include "logwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QTextEdit>

LogWidget::LogWidget(QWidget *parent)
    : QWidget{parent}
{
    createInterface();

    for(int i = 0; i < 20; ++i)
        addLog("text info " + QString::number(i), LogWidget::INFO);
    addLog("text success", LogWidget::SUCCESS);
    addLog("text danger", LogWidget::DANGER);
    addLog("text warning", LogWidget::WARNING);
}

void LogWidget::createInterface()
{
    setFixedWidth(450);

    // Ajouter un layout avec un bouton comme contenu du widget
    QVBoxLayout *layout = new QVBoxLayout(this);

    d_logtext = new QTextEdit{};
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

    // Appliquer l'effet d'ombre
//    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
//    shadowEffect->setBlurRadius(60);       // Rayon de flou pour l'ombre
//    shadowEffect->setOffset(5, 5);         // Décalage de l'ombre
//    shadowEffect->setColor(Qt::black);     // Couleur de l'ombre (ici noir)
//    setGraphicsEffect(shadowEffect);       // Applique l'effet d'ombre au widget
}

bool LogWidget::addLog(const QString& text, int type)
{
    QString color = "#FFFFFF";
    QString label = "[INFO]";


    switch (type) {
    case LogWidget::SUCCESS:
        color = "#34d399";
        label = "[SUCCESS]";
        break;
    case LogWidget::WARNING:
        color = "#fed7aa";
        label = "[WARNING]";
        break;
    case LogWidget::DANGER:
        color = "#F87171";
        label = "[DANGER]";
        break;
    }

    QString log = "<span style='margin-bottom:20px; color: " + color + ";'>" + label + " " + text + "</span>";
    d_logtext->append(log);
    return true;
}
