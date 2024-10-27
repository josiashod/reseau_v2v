QT       += core gui sql concurrent

LIBS += -lproj

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    core/building.cpp \
    core/mapitem.cpp \
    core/node.cpp \
    core/noded.cpp \
    core/park.cpp \
    core/water.cpp \
    core/way.cpp \
    main.cpp \
    ui/widgets/mapviewer.cpp \
    ui/windows/mainwindow.cpp \
    utils/dbmanager.cpp \
    utils/osmreader.cpp \

HEADERS += \
    core/building.h \
    core/mapitem.h \
    core/node.h \
    core/noded.h \
    core/park.h \
    core/water.h \
    core/way.h \
    ui/widgets/mapviewer.h \
    ui/windows/mainwindow.h \
    utils/dbmanager.h \
    utils/osmreader.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    lib/geo.db \
    lib/mulhouse.osm

RESOURCES += \
    app.qrc
