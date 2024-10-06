QT       += core gui sql

LIBS += -lproj

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dbmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    mapitem.cpp \
    building.cpp \
    park.cpp \
    water.cpp \
    way.cpp \
    mapviewer.cpp \
    node.cpp \
    noded.cpp \
    osmreader.cpp

HEADERS += \
    dbmanager.h \
    mainwindow.h \
    mapitem.h \
    building.h \
    park.h \
    water.h \
    way.h \
    mapviewer.h \
    node.h \
    noded.h \
    osmreader.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    lib/mulhouse.osm

RESOURCES += \
    app.qrc
