#-------------------------------------------------
#
# Project created by QtCreator 2014-07-14T15:30:34
#
#-------------------------------------------------

QT       += xml xmlpatterns testlib

TARGET = tst_verilogport
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_verilogport.cpp \
    ../../../../kactusGenerators/verilogGenerator/VerilogPort.cpp \
    ../../../../kactusGenerators/HDLGenerator/HDLObject.cpp \
    ../../../../kactusGenerators/HDLGenerator/HDLPort.cpp \
    ../../../../kactusGenerators/HDLGenerator/HDLUtils.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../executable/ -lKactus2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../executable/ -lKactus2_d
else:unix:!macx: LIBS += -L$$PWD/../../../../executable/ -lKactus2

INCLUDEPATH += $$PWD/../../../../
DEPENDPATH += $$PWD/../../../../

HEADERS += \
    ../../../../kactusGenerators/verilogGenerator/VerilogPort.h \
    ../../../../kactusGenerators/HDLGenerator/HDLObject.h \
    ../../../../kactusGenerators/HDLGenerator/HDLPort.h \
    ../../../../kactusGenerators/HDLGenerator/HDLUtils.h
