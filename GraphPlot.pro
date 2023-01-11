QT       += core gui charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

SOURCES += \
    src/CalculateCylinder.cpp \
    src/UniqueGraph.cpp \
    src/PLC.cpp \
    src/Sensor.cpp \
    src/Utilites.cpp \
    src/main.cpp \
    src/GraphPlot.cpp

HEADERS += \
    include/CalculateCylinder.h \
    include/GraphPlot.h \
    include/UniqueGraph.h \
    include/PLC.h \
    include/Sensor.h \
    include/Utilites.h

FORMS += \
    res/GraphPlot.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    TODO

RESOURCES += \
    res/pic.qrc

win32: LIBS += -L$$PWD/./lib/ -lsnap7

INCLUDEPATH += $$PWD/include/.
INCLUDEPATH += $$PWD/lib/.
DEPENDPATH += $$PWD/lib/.
