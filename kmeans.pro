QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Controls3D.cpp \
    Info.cpp \
    RandomData.cpp \
    ViewWidget.cpp \
    kmeans.cpp \
    main.cpp \
    MainWindow.cpp \
    qcustomplot.cpp

HEADERS += \
    Controls3D.h \
    Info.h \
    MainWindow.h \
    RandomData.h \
    ViewWidget.h \
    kmeans.h \
    qcustomplot.h

FORMS += \
    Controls3D.ui \
    Info.ui \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
