HEADERS += \
    ../include/msg_types.h \
    ../include/jsonHelper.h \
    ../include/C_R.h \
    server_console.h \
    msgcom.h \
    ../include/Handshake.h \
    ../include/Errors.h \
    ../include/Communicate.h \
    ../include/base64.h \
    ../include/md5.h \
    ../include/utf8.h \
    ../include/sha1.h \
    ../include/Datastructures.h \
    usb.h \
    ../include/S-AES.h

SOURCES += \
    server_multi.cpp \
    ../include/Handshake.c \
    ../include/Errors.c \
    ../include/Communicate.c \
    ../include/base64.c \
    ../include/Datastructures.c \
    ../include/md5.c \
    ../include/utf8.c \
    ../include/sha1.c \
    usb.cpp \
    ../include/S-AES.c

unix:!macx: LIBS += -L$$PWD/../lib/ -ljsoncpp

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include
