#include "widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include "../ui/widgets/logwidget.h"
#include "../utils/osmreader.h"

Widget::Widget(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle("Reseau_2V2 DB Loader");
    setFixedSize(700, 700);

    auto layout = new QVBoxLayout{this};
    auto hlayout = new QHBoxLayout{};

    d_edit = new QLineEdit{};
    d_edit->setEnabled(false);
    auto btn = new QPushButton{"Charger le fichier"};
    auto btn_load = new QPushButton{"Remplir la BD"};

    hlayout->addWidget(d_edit, 1);
    hlayout->addWidget(btn);
    hlayout->addWidget(btn_load);
    setLayout(layout);
    layout->addLayout(hlayout);
    layout->addWidget(LogWidget::instance(), 1);

    connect(btn, &QPushButton::clicked, this, &Widget::onLoadFile);
    connect(btn_load, &QPushButton::clicked, this, &Widget::onLoadDB);
}

void Widget::onLoadFile()
{
    QString directory = QDir::homePath();
    QLocale locale = QLocale();

    directory += "/" + QStandardPaths::displayName(QStandardPaths::DocumentsLocation) + "/";

    QString fileName = QFileDialog::getOpenFileName(this,
                        tr("Importer le fichier OSM à charger"),
                        directory);

    d_edit->setText(fileName);
}

void Widget::onLoadDB()
{
    LogWidget::clear();
    OsmReader::readOSMFile(d_edit->text());
}

