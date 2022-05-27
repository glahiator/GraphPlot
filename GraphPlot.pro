QT       += core gui charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CalculateCylinder.cpp \
    Graphs/UniqueGraph.cpp \
    PLC_Connector.cpp \
    Sensor.cpp \
    Utilites.cpp \
    main.cpp \
    GraphPlot.cpp \
#    snap7.cpp

HEADERS += \
    CalculateCylinder.h \
    GraphPlot.h \
    Graphs/UniqueGraph.h \
    PLC_Connector.h \
    Sensor.h \
    Utilites.h \
#    snap7.h

FORMS += \
    GraphPlot.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    TODO

RESOURCES += \
    pic.qrc

win32: LIBS += -L$$PWD/./ -lsnap7

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
