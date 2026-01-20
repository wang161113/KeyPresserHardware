QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# 设置编码
CODECFORTR = UTF-8
CODECFORSRC = UTF-8

CONFIG += c++17
QMAKE_PROJECT_DEPTH = 0
#win32:CONFIG(release, debug|release) {
#    QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
#}
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
win32:LIBS += -lgdi32

SOURCES += \
    aboutmedlg.cpp \
    keypresserHardware.cpp \
    main.cpp


HEADERS += \
    ArduinoController.hpp \
    aboutmedlg.h \
    keypresserHardware.h


# 添加 User32.lib 库链接
LIBS += -luser32
RC_ICONS = ./keypresser.ico
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    res.qrc

FORMS += \
    aboutmedlg.ui

DISTFILES += \
    style.qss

