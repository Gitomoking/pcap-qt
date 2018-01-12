#-------------------------------------------------
#
# Project created by QtCreator 2017-01-06T17:56:12
#
#-------------------------------------------------

QT       += core gui charts sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pcap-qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    vpacket.cpp \
    vnode.cpp \
    vinterface.cpp \
    visualizationwindow.cpp \
    filterdialog.cpp \
    filterexpressionwindow.cpp \
    logwindow.cpp \
    protocolwindow.cpp \
    pieview.cpp \
    trafficwindow.cpp \
    nodesetwindow.cpp \
    dbaccess.cpp \
    searchdialog.cpp \
    devicewindow.cpp \
    logsavedialog.cpp

HEADERS  += mainwindow.h \
    vpacket.h \
    vnode.h \
    vinterface.h \
    visualizationwindow.h \
    filterdialog.h \
    filterexpressionwindow.h \
    logwindow.h \
    protocolwindow.h \
    pieview.h \
    trafficwindow.h \
    dbaccess.h \
    nodesetwindow.h \
    searchdialog.h \
    devicewindow.h \
    logsavedialog.h

FORMS    +=

RESOURCES += \
    images.qrc
