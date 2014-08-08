unix:!macx: LIBS += -L$$PWD/../lib/ -ljsoncpp

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include


HEADERS += \
    ../include/msg_types.h \
    ../include/jsonHelper.h \
    ../include/C_R.h \
    msgcom.h \
    client_console.h

SOURCES += \
    client.cpp
