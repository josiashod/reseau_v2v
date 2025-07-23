#include "widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QApplication>

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
    LogWidget::init(this); // Ensure LogWidget is initialized with parent
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

    if(!fileName.isEmpty()) {
        d_edit->setText(fileName);
    }
}

void Widget::onLoadDB()
{
    if(d_edit->text().isEmpty()) {
        LogWidget::addLog(tr("Veuillez d'abord sélectionner un fichier OSM"), LogWidget::WARNING);
        return;
    }

    if(!QFile::exists(d_edit->text())) {
        LogWidget::addLog(tr("Le fichier spécifié n'existe pas"), LogWidget::DANGER);
        return;
    }
    LogWidget::clear();

    // Ajouter un feedback visuel (curseur en attente)
    QApplication::setOverrideCursor(Qt::WaitCursor);
    try {
        OsmReader::readOSMFile(d_edit->text());
        LogWidget::addLog(tr("Le fichier OSM a été chargé avec succès"),  LogWidget::INFO);
    } catch(const std::exception& e) {
        LogWidget::addLog(tr("Erreur lors du chargement: %1").arg(e.what()),  LogWidget::DANGER);
    }
    QApplication::restoreOverrideCursor();
}

