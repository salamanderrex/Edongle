HEADERS += \
    ../include/msg_types.h \
    ../include/jsonHelper.h \
    ../include/C_R.h \
    server_console.h

SOURCES += \
    server_multi.cpp

unix:!macx: LIBS += -L$$PWD/../lib/ -ljsonlib

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include
