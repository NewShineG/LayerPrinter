QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_ICONS = "LP.ico"

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DMDLib/API.c \
    DMDLib/BMPParser.c \
    DMDLib/Error.c \
    DMDLib/batchfile.c \
    DMDLib/compress.c \
    DMDLib/firmware.c \
    DMDLib/flashimage.c \
    DMDLib/flashloader.c \
    DMDLib/pattern.c \
    DMDLib/splash.c \
    DMDLib/usb.c \
    PIStage.cpp \
    dmd.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    DMDLib/API.h \
    DMDLib/BMPParser.h \
    DMDLib/Error.h \
    DMDLib/batchfile.h \
    DMDLib/common.h \
    DMDLib/compress.h \
    DMDLib/firmware.h \
    DMDLib/flashimage.h \
    DMDLib/flashloader.h \
    DMDLib/pattern.h \
    DMDLib/splash.h \
    DMDLib/usb.h \
    PI_GCS_2/PI_GCS2_DLL.h \
    PtnImage.h \
    hidapi/hidapi.h \
    mainwindow.h \
    patternelement.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    LayerPrinter.qrc \
    LayerPrinter.qrc
