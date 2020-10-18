#-------------------------------------------------
#
# Project created by QtCreator 2019-08-17T14:26:54
#
#-------------------------------------------------

QT       += core gui qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QMudEditor
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

CONFIG += c++11

unix:LIBS += $${_PRO_FILE_PWD_}/QCodeEditor/build/libQCodeEditor.a
INCLUDEPATH += $${_PRO_FILE_PWD_}/QCodeEditor/include

CONFIG(debug, debug|release) {
    win32:LIBS += $${_PRO_FILE_PWD_}/QCodeEditor/build/Debug/QCodeEditor.lib
} else {
    win32:LIBS += $${_PRO_FILE_PWD_}/QCodeEditor/build/Release/QCodeEditor.lib
}


SOURCES += \
    editquestdialog.cpp \
    editquesttargetdialog.cpp \
    formtilesmapeditor.cpp \
    graphicsviewzoom.cpp \
        main.cpp \
        mainwindow.cpp \
    ../common.cpp \
    editormap.cpp \
    formcenteredcheckbox.cpp \
    editornpc.cpp \
    editoritem.cpp \
    scripteditordialog.cpp \
    dialogeditcolors.cpp \
    editexitdialog.cpp \
    selectfromlistdialog.cpp \
    treewidgetaction.cpp

HEADERS += \
    editquestdialog.h \
    editquesttargetdialog.h \
    formtilesmapeditor.h \
    graphicsviewzoom.h \
        mainwindow.h \
    ../common.h \
    editormap.h \
    formcenteredcheckbox.h \
    editornpc.h \
    editoritem.h \
    scripteditordialog.h \
    dialogeditcolors.h \
    editexitdialog.h \
    selectfromlistdialog.h \
    treewidgetaction.h

FORMS += \
    editquestdialog.ui \
    editquesttargetdialog.ui \
    formtilesmapeditor.ui \
        mainwindow.ui \
    formcenteredcheckbox.ui \
    dialogeditcolors.ui \
    editexitdialog.ui \
    scripteditordialog.ui \
    selectfromlistdialog.ui \
    treewidgetaction.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
