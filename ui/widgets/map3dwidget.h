#ifndef MAP3DWIDGET_H
#define MAP3DWIDGET_H

#include <QColor>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QPointF>
#include <QRectF>
#include <QVector>
#include <QVector3D>
#include <QVector4D>

#include "utils/osmreader.h"

class Car;
class QMouseEvent;
class QWheelEvent;

class Map3DWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit Map3DWidget(QWidget* parent = nullptr);
    ~Map3DWidget() override;

    void setParsedMap(const ParsedOsmMap& map);
    void setSceneRect(const QRectF& sceneRect);
    void setCars(const QVector<Car*>& cars);
    void setShowPark(bool show);
    void setShowBuilding(bool show);
    void setShowRoad(bool show);
    void setShowRadioCoverage(bool show);

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    struct Vertex
    {
        QVector3D position;
        QVector4D color;
    };

    QVector3D nodeWorldPosition(long long nodeId, double z = 0.0) const;
    QVector3D sceneWorldPosition(const QPointF& point, double z = 0.0) const;
    QVector4D glColor(const QColor& color) const;

    void drawGround();
    void drawFeature(const OsmFeature& feature, const QColor& color, double height = 0.0);
    void drawRoad(const OsmFeature& feature);
    void drawCars();
    void drawRadioCoverage(const Car* car);
    void drawCarBody(const Car* car);
    void drawVertices(GLenum primitive, const QVector<Vertex>& vertices);

    ParsedOsmMap d_map;
    QVector<Car*> d_cars;
    std::pair<double, double> d_minCoord{0.0, 0.0};
    std::pair<double, double> d_maxCoord{1.0, 1.0};
    QRectF d_sceneRect{0.0, 0.0, 1.0, 1.0};
    QPoint d_lastMousePos;
    QVector3D d_pan{0.0f, 0.0f, 0.0f};
    double d_zoom = 1.0;
    double d_yaw = -32.0;
    double d_pitch = 58.0;
    bool d_showPark = true;
    bool d_showBuilding = true;
    bool d_showRoad = true;
    bool d_showRadioCoverage = true;

    QOpenGLShaderProgram* d_program = nullptr;
    QOpenGLBuffer d_vbo{QOpenGLBuffer::VertexBuffer};
    QMatrix4x4 d_mvp;
};

#endif // MAP3DWIDGET_H
