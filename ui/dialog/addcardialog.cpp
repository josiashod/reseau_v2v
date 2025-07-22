#include "addcardialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QDial>
#include <QPushButton>
#include <QMessageBox>

AddCarDialog::AddCarDialog(QWidget *parent)
    : QDialog{parent}
{
    create_interface();
}

void AddCarDialog::create_interface()
{
    setWindowTitle("Ajouter des voitures");
    setMinimumSize(450, 100);
    setAttribute(Qt::WA_DeleteOnClose);

    auto main_layout = new QVBoxLayout(this);

    main_layout->addWidget(new QLabel{"Nombre de voitures à ajouter"});
    d_nm_car_edit= new QLineEdit();

    d_nm_car_edit->setValidator(new QIntValidator(0, 99, d_nm_car_edit));
    d_nm_car_edit->setPlaceholderText("Enter the number of car to add");

    main_layout->addWidget(d_nm_car_edit);
    main_layout->addSpacing(10);

    auto dial_layout = new QHBoxLayout();

    auto speed_dial_layout = new QVBoxLayout();
    auto freq_dial_layout = new QVBoxLayout();
    auto intensity_dial_layout = new QVBoxLayout();


    // ajout des etiquette pour les molletes
    speed_dial_layout->addWidget(new QLabel{"Vitesse"}, 0, Qt::AlignCenter);
    freq_dial_layout->addWidget(new QLabel{"Fréquence"}, 0, Qt::AlignCenter);
    intensity_dial_layout->addWidget(new QLabel{"Puissance"}, 0, Qt::AlignCenter);

    // créations des molette de frequence et de puissance
    // avec une valeur par defaut à 20
    d_speed_dial = new QDial(this);
    d_speed_dial->setMinimum(80.0);
    d_speed_dial->setMaximum(300.0);
    d_speed_dial->setNotchesVisible(true);

    d_freq_dial = new QDial(this);
    d_freq_dial->setMinimum(45.0);
    d_freq_dial->setMaximum(180.0);
    d_freq_dial->setNotchesVisible(true);

    d_intensity_dial = new QDial(this);
    d_intensity_dial->setMinimum(35.0);
    d_intensity_dial->setMaximum(100.0);
    d_intensity_dial->setNotchesVisible(true);

    speed_dial_layout->addWidget(d_speed_dial);
    freq_dial_layout->addWidget(d_freq_dial);
    intensity_dial_layout->addWidget(d_intensity_dial);

    // etiquette pour afficher la valeur selectionner par les différentes molletes
    d_speed_label = new QLabel{QString::number(d_speed_dial->value())};
    d_freq_label = new QLabel{QString::number(d_freq_dial->value())};
    d_intensity_label = new QLabel{QString::number(d_intensity_dial->value())};

    // ajout des etiquette pour les molletes
    speed_dial_layout->addWidget(d_speed_label, 0, Qt::AlignCenter);
    freq_dial_layout->addWidget(d_freq_label, 0, Qt::AlignCenter);
    intensity_dial_layout->addWidget(d_intensity_label, 0, Qt::AlignCenter);

    connect(d_speed_dial, &QDial::valueChanged, [this](int value) {
        d_speed_label->setText(QString::number(value));
    });

    connect(d_freq_dial, &QDial::valueChanged, [this](int value) {
        d_freq_label->setText(QString::number(value));
    });

    connect(d_intensity_dial, &QDial::valueChanged, [this](int value) {
        d_intensity_label->setText(QString::number(value));
    });

    auto vline1 {new QFrame{this}};
    vline1->setFrameStyle(QFrame::VLine|QFrame::Sunken);

    dial_layout->addLayout(speed_dial_layout);
    dial_layout->addWidget(vline1);
    dial_layout->addLayout(freq_dial_layout);

    auto vline2 {new QFrame{this}};
    vline2->setFrameStyle(QFrame::VLine|QFrame::Sunken);

    dial_layout->addWidget(vline2);
    dial_layout->addLayout(intensity_dial_layout);

    main_layout->addLayout(dial_layout);

    auto hline {new QFrame{this}};
    hline->setFrameStyle(QFrame::HLine|QFrame::Sunken);
    main_layout->addWidget(hline);


    // bouton de validation
    auto btn{new QPushButton{"Valider"}};
    main_layout->addWidget(btn);

    // mise en place de l'ecoute d'evenement
    connect(btn, &QPushButton::clicked, this, &AddCarDialog::onAdd);
}

void AddCarDialog::onAdd()
{
    if(d_nm_car_edit->text().length() == 0)
    {
        QMessageBox::critical(this, "Erreur", "Veuillez remplir correctement les différentes informations");
        return;
    }
    int nb = d_nm_car_edit->text().toInt();
    double speed = d_speed_dial->value();
    double freq = d_freq_dial->value();
    double intensity = d_intensity_dial->value();

    emit create_car(nb, speed, freq, intensity);
    close();
}
