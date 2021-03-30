QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    filesystem.cpp \
    inputcontroldelegate.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    IFileSystem.h \
    filesystem.h \
    inputcontroldelegate.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../SimplexLib/release/ -lSimplexLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../SimplexLib/debug/ -lSimplexLib
else:unix: LIBS += -L$$PWD/../SimplexLib/ -lSimplexLib

INCLUDEPATH += $$PWD/../SimplexLib
DEPENDPATH += $$PWD/../SimplexLib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Chart/release/ -lChart
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Chart/debug/ -lChart
else:unix: LIBS += -L$$PWD/../Chart/ -lChart

INCLUDEPATH += $$PWD/../Chart
DEPENDPATH += $$PWD/../Chart
