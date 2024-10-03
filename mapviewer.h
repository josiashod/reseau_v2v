#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QWidget>
#include <QPainter>
#include <QPoint>

class MapViewer : public QWidget
{
    Q_OBJECT
public:
    explicit MapViewer(QWidget *parent = nullptr);
    ~MapViewer() = default;
    /**
     * @brief Redéfinition de la méthode sizeHint - On redimensionne le QWidget par défaut
     * @return
     */
    QSize sizeHint() const override;
    void dessinePolygone(QPainter &painter);

private:
    /**
     * @brief Méthode dessinant le map
     */
    void paintEvent(QPaintEvent *) override;
public slots :
    void map_update();
};

#endif // MAPVIEWER_H
