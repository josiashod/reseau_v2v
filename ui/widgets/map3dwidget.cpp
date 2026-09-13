#include "map3dwidget.h"

#include "core/car.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>

namespace
{
constexpr float kGroundSize = 1400.0f;
constexpr double kBuildingHeight = 24.0;
constexpr int kDiskSegments = 72;

QVector4D colorToVector(const QColor& color)
{
    return {
        color.redF(),
        color.greenF(),
        color.blueF(),
        color.alphaF()
    };
}
}

Map3DWidget::Map3DWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setMouseTracking(true);
}

Map3DWidget::~Map3DWidget()
{
    makeCurrent();
    d_vbo.destroy();
    delete d_program;
    d_program = nullptr;
    doneCurrent();
}

void Map3DWidget::setParsedMap(const ParsedOsmMap& map)
{
    d_map = map;
    if(d_map.bounds.valid) {
        d_minCoord = {d_map.bounds.minLon, d_map.bounds.minLat};
        d_maxCoord = {d_map.bounds.maxLon, d_map.bounds.maxLat};
    }
    update();
}

void Map3DWidget::setSceneRect(const QRectF& sceneRect)
{
    d_sceneRect = sceneRect;
    update();
}

void Map3DWidget::setCars(const QVector<Car*>& cars)
{
    d_cars = cars;
    update();
}

void Map3DWidget::setShowPark(bool show)
{
    d_showPark = show;
    update();
}

void Map3DWidget::setShowBuilding(bool show)
{
    d_showBuilding = show;
    update();
}

void Map3DWidget::setShowRoad(bool show)
{
    d_showRoad = show;
    update();
}

void Map3DWidget::setShowRadioCoverage(bool show)
{
    d_showRadioCoverage = show;
    update();
}

void Map3DWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.95f, 0.94f, 0.90f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    d_program = new QOpenGLShaderProgram{this};
    d_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
        "attribute highp vec3 position;\n"
        "attribute lowp vec4 color;\n"
        "uniform highp mat4 mvp;\n"
        "varying lowp vec4 vColor;\n"
        "void main() {\n"
        "    vColor = color;\n"
        "    gl_Position = mvp * vec4(position, 1.0);\n"
        "}\n");
    d_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
        "varying lowp vec4 vColor;\n"
        "void main() {\n"
        "    gl_FragColor = vColor;\n"
        "}\n");
    d_program->link();
    d_vbo.create();
}

void Map3DWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Map3DWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 projection;
    projection.perspective(45.0f, std::max(1.0f, width() / static_cast<float>(std::max(1, height()))), 1.0f, 5000.0f);

    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, static_cast<float>(-1500.0 / d_zoom));
    view.rotate(static_cast<float>(d_pitch), 1.0f, 0.0f, 0.0f);
    view.rotate(static_cast<float>(d_yaw), 0.0f, 0.0f, 1.0f);
    view.translate(d_pan);

    d_mvp = projection * view;

    drawGround();

    if(d_showPark) {
        for(const auto& park: d_map.parks)
            drawFeature(park, QColor{83, 159, 103, 170});
    }

    for(const auto& water: d_map.waters)
        drawFeature(water, QColor{69, 147, 190, 190}, 0.4);

    if(d_showBuilding) {
        for(const auto& building: d_map.buildings)
            drawFeature(building, QColor{178, 165, 146, 255}, kBuildingHeight);
    }

    if(d_showRoad) {
        glLineWidth(3.0f);
        for(const auto& road: d_map.roads)
            drawRoad(road);
    }

    drawCars();
}

void Map3DWidget::wheelEvent(QWheelEvent* event)
{
    const double factor = event->angleDelta().y() > 0 ? 1.12 : 1.0 / 1.12;
    d_zoom = std::clamp(d_zoom * factor, 0.35, 8.0);
    update();
}

void Map3DWidget::mousePressEvent(QMouseEvent* event)
{
    d_lastMousePos = event->pos();
}

void Map3DWidget::mouseMoveEvent(QMouseEvent* event)
{
    const QPoint delta = event->pos() - d_lastMousePos;
    d_lastMousePos = event->pos();

    if(event->buttons() & Qt::LeftButton) {
        d_pan += QVector3D{delta.x() * 1.5f, -delta.y() * 1.5f, 0.0f};
    }
    else if(event->buttons() & Qt::RightButton) {
        d_yaw += delta.x() * 0.35;
        d_pitch = std::clamp(d_pitch + delta.y() * 0.25, 25.0, 78.0);
    }

    update();
}

QVector3D Map3DWidget::nodeWorldPosition(long long nodeId, double z) const
{
    auto it = d_map.nodes.find(nodeId);
    if(it == d_map.nodes.end())
        return {0.0f, 0.0f, static_cast<float>(z)};

    const double lonSpan = std::max(0.000001, d_maxCoord.first - d_minCoord.first);
    const double latSpan = std::max(0.000001, d_maxCoord.second - d_minCoord.second);
    const double x = (it->second.first - d_minCoord.first) / lonSpan * d_sceneRect.width();
    const double y = d_sceneRect.height() - (it->second.second - d_minCoord.second) / latSpan * d_sceneRect.height();

    return sceneWorldPosition({x, y}, z);
}

QVector3D Map3DWidget::sceneWorldPosition(const QPointF& point, double z) const
{
    const double scale = kGroundSize / std::max(1.0, std::max(d_sceneRect.width(), d_sceneRect.height()));
    return {
        static_cast<float>((point.x() - d_sceneRect.center().x()) * scale),
        static_cast<float>((point.y() - d_sceneRect.center().y()) * scale),
        static_cast<float>(z)
    };
}

QVector4D Map3DWidget::glColor(const QColor& color) const
{
    return colorToVector(color);
}

void Map3DWidget::drawGround()
{
    const QVector4D color = glColor(QColor{"#F2EFE9"});
    QVector<Vertex> vertices{
        {{-kGroundSize / 2.0f, -kGroundSize / 2.0f, -1.0f}, color},
        {{ kGroundSize / 2.0f, -kGroundSize / 2.0f, -1.0f}, color},
        {{ kGroundSize / 2.0f,  kGroundSize / 2.0f, -1.0f}, color},
        {{-kGroundSize / 2.0f, -kGroundSize / 2.0f, -1.0f}, color},
        {{ kGroundSize / 2.0f,  kGroundSize / 2.0f, -1.0f}, color},
        {{-kGroundSize / 2.0f,  kGroundSize / 2.0f, -1.0f}, color},
    };

    drawVertices(GL_TRIANGLES, vertices);
}

void Map3DWidget::drawFeature(const OsmFeature& feature, const QColor& color, double height)
{
    QVector<QVector3D> ground;
    QVector<QVector3D> top;
    ground.reserve(feature.nodeRefs.size());
    top.reserve(feature.nodeRefs.size());

    for(long long nodeId: feature.nodeRefs) {
        ground.push_back(nodeWorldPosition(nodeId, 0.0));
        if(height > 0.0)
            top.push_back(nodeWorldPosition(nodeId, height));
    }

    if(ground.size() < 3)
        return;

    QVector<Vertex> vertices;
    const QVector4D topColor = glColor(color);

    for(int i = 1; i + 1 < ground.size(); ++i) {
        vertices.push_back({height > 0.0 ? top[0] : ground[0], topColor});
        vertices.push_back({height > 0.0 ? top[i] : ground[i], topColor});
        vertices.push_back({height > 0.0 ? top[i + 1] : ground[i + 1], topColor});
    }

    if(height > 0.0 && top.size() == ground.size()) {
        QColor sideColor = color.darker(125);
        sideColor.setAlpha(255);
        const QVector4D side = glColor(sideColor);

        for(int i = 0; i < ground.size(); ++i) {
            const int next = (i + 1) % ground.size();
            vertices.push_back({ground[i], side});
            vertices.push_back({ground[next], side});
            vertices.push_back({top[next], side});
            vertices.push_back({ground[i], side});
            vertices.push_back({top[next], side});
            vertices.push_back({top[i], side});
        }
    }

    drawVertices(GL_TRIANGLES, vertices);
}

void Map3DWidget::drawRoad(const OsmFeature& feature)
{
    QVector<Vertex> vertices;
    vertices.reserve(feature.nodeRefs.size());
    const QVector4D color = glColor(QColor{"#FED7A1"});

    for(long long nodeId: feature.nodeRefs)
        vertices.push_back({nodeWorldPosition(nodeId, 1.2), color});

    if(vertices.size() >= 2)
        drawVertices(GL_LINE_STRIP, vertices);
}

void Map3DWidget::drawCars()
{
    for(Car* car: d_cars) {
        if(!car)
            continue;

        if(d_showRadioCoverage)
            drawRadioCoverage(car);

        drawCarBody(car);
    }
}

void Map3DWidget::drawRadioCoverage(const Car* car)
{
    QColor color = car->color();
    color.setAlphaF(0.22);
    const QVector4D fill = glColor(color);
    const QVector3D center = sceneWorldPosition(car->pos(), 1.8);
    const double scale = kGroundSize / std::max(1.0, std::max(d_sceneRect.width(), d_sceneRect.height()));
    const float radius = static_cast<float>(car->coverageRadius() * scale);

    QVector<Vertex> vertices;
    vertices.reserve(kDiskSegments + 2);
    vertices.push_back({center, fill});

    for(int i = 0; i <= kDiskSegments; ++i) {
        const float a = static_cast<float>(2.0 * M_PI * i / kDiskSegments);
        vertices.push_back({center + QVector3D{std::cos(a) * radius, std::sin(a) * radius, 0.0f}, fill});
    }

    drawVertices(GL_TRIANGLE_FAN, vertices);
}

void Map3DWidget::drawCarBody(const Car* car)
{
    QColor bodyColor = car->color();
    bodyColor.setAlpha(255);
    const QVector4D color = glColor(bodyColor);
    const QVector3D c = sceneWorldPosition(car->pos(), 10.0);
    const float w = 7.0f;
    const float h = 4.0f;
    const float z = 5.0f;

    QVector<Vertex> vertices{
        {c + QVector3D{-w, -h, 0}, color},
        {c + QVector3D{ w, -h, 0}, color},
        {c + QVector3D{ w,  h, 0}, color},
        {c + QVector3D{-w, -h, 0}, color},
        {c + QVector3D{ w,  h, 0}, color},
        {c + QVector3D{-w,  h, 0}, color},

        {c + QVector3D{-w, -h, 0}, color},
        {c + QVector3D{ w, -h, 0}, color},
        {c + QVector3D{0, 0, z}, color},
        {c + QVector3D{ w, -h, 0}, color},
        {c + QVector3D{ w,  h, 0}, color},
        {c + QVector3D{0, 0, z}, color},
        {c + QVector3D{ w,  h, 0}, color},
        {c + QVector3D{-w,  h, 0}, color},
        {c + QVector3D{0, 0, z}, color},
        {c + QVector3D{-w,  h, 0}, color},
        {c + QVector3D{-w, -h, 0}, color},
        {c + QVector3D{0, 0, z}, color},
    };

    drawVertices(GL_TRIANGLES, vertices);
}

void Map3DWidget::drawVertices(GLenum primitive, const QVector<Vertex>& vertices)
{
    if(vertices.empty() || !d_program)
        return;

    d_program->bind();
    d_program->setUniformValue("mvp", d_mvp);

    d_vbo.bind();
    d_vbo.allocate(vertices.constData(), static_cast<int>(vertices.size() * sizeof(Vertex)));

    const int positionLocation = d_program->attributeLocation("position");
    const int colorLocation = d_program->attributeLocation("color");

    d_program->enableAttributeArray(positionLocation);
    d_program->enableAttributeArray(colorLocation);
    d_program->setAttributeBuffer(positionLocation, GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));
    d_program->setAttributeBuffer(colorLocation, GL_FLOAT, offsetof(Vertex, color), 4, sizeof(Vertex));

    glDrawArrays(primitive, 0, vertices.size());

    d_program->disableAttributeArray(positionLocation);
    d_program->disableAttributeArray(colorLocation);
    d_vbo.release();
    d_program->release();
}
