#-------------------------------------------------
#
# Project created by QtCreator 2018-01-10T12:36:31
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MposOperators
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    loggingcategories.cpp \
    logindialog.cpp \
    usersdialog.cpp \
    dbaseconnect.cpp \
    newoperatordialog.cpp \
    operatorchanged.cpp \
    connectionlistdialog.cpp

HEADERS += \
        mainwindow.h \
    databases.h \
    loggingcategories.h \
    logindialog.h \
    usersdialog.h \
    dbaseconnect.h \
    operatorsdata.h \
    newoperatordialog.h \
    operatorchanged.h \
    connectionlistdialog.h

FORMS += \
        mainwindow.ui \
    logindialog.ui \
    usersdialog.ui \
    newoperatordialog.ui \
    connectionlistdialog.ui

DISTFILES +=

RESOURCES += \
    res.qrc \

