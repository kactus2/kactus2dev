#-------------------------------------------------
#
# Project created by QtCreator 2014-07-14T12:19:19
#
#-------------------------------------------------

QT       += xml xmlpatterns testlib

TARGET = tst_verilogparameter
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    ../../../../kactusGenerators/verilogGenerator/VerilogParameter.cpp \
    ../../../../kactusGenerators/HDLGenerator/HDLModelParameter.cpp \
    ../../../../kactusGenerators/HDLGenerator/HDLObject.cpp \
    tst_verilogparameter.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../executable/ -lKactus2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../executable/ -lKactus2_d
else:unix: LIBS += -L$$PWD/../../../../executable/ -lKactus2

INCLUDEPATH += $$PWD/../../../../
DEPENDPATH += $$PWD/../../../../

HEADERS += \
    ../../../../kactusGenerators/verilogGenerator/VerilogParameter.h \
    ../../../../kactusGenerators/HDLGenerator/HDLModelParameter.h \
    ../../../../kactusGenerators/HDLGenerator/HDLObject.h
