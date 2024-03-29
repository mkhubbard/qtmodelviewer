# #####################################################################
# Automatically generated by qmake (2.01a) Thu Mar 12 20:40:58 2009
# #####################################################################
TEMPLATE = app
TARGET = 
DEPENDPATH += . \
    qtc-gdbmacros \
    resources
INCLUDEPATH += .
CONFIG += console \
    debug
QT += opengl

RC_FILE=resources/mv.rc
ICON=resources/mv.icns

# Input
HEADERS += MainWindow.h \
    OgreRenderWindow.h \
    Mesh.h \
    Common.h \
    Grid.h \
    MvApplication.h \
    Timeline.h \
    ProxyStyle.h \
    AnimationFrame.h
FORMS += MainWindow.ui \
    AnimationFrame.ui
SOURCES += Main.cpp \
    MainWindow.cpp \
    OgreRenderWindow.cpp \
    qtc-gdbmacros/gdbmacros.cpp \
    Mesh.cpp \
    Grid.cpp \
    MvApplication.cpp \
    Timeline.cpp \
    ProxyStyle.cpp \
    AnimationFrame.cpp
RESOURCES += resources/resources.qrc
