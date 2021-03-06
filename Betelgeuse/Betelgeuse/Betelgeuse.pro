#-------------------------------------------------
#
# Project created by QtCreator 2016-12-17T12:42:43
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Betelgeuse
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

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

CONFIG(debug, debug|release) {
    DEFINES += DEVMODE
}

SOURCES += main.cpp \
    dmwindow.cpp \
    generalscripteditor.cpp \
    yesnocanceldialog.cpp \
    tablemanager.cpp \
    choosesearchdialog.cpp \
    modifiablecombolist.cpp \
    rolltableresult.cpp \
    searchparameter.cpp \
    tableeditor.cpp \
    tablecolumneditor.cpp \
    rolltableselector.cpp \
    rollmultipletables.cpp \
    multiplerolltableresults.cpp \
    dieroller.cpp

HEADERS  += \
    dmwindow.h \
    initdb.h \
    generalscripteditor.h \
    yesnocanceldialog.h \
    tablemanager.h \
    choosesearchdialog.h \
    modifiablecombolist.h \
    rolltableresult.h \
    searchparameter.h \
    tablecolumneditor.h \
    tableeditor.h \
    rolltableselector.h \
    rollmultipletables.h \
    multiplerolltableresults.h \
    dieroller.h

FORMS    += \
    dmwindow.ui \
    generalscripteditor.ui \
    yesnocanceldialog.ui \
    tablemanager.ui \
    choosesearchdialog.ui \
    modifiablecombolist.ui \
    rolltableresult.ui \
    searchparameter.ui \
    tablecolumneditor.ui \
    tableeditor.ui \
    rolltableselector.ui \
    rollmultipletables.ui \
    multiplerolltableresults.ui \
    dieroller.ui
