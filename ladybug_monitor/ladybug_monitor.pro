QT       += core gui widgets opengl

TARGET = ladybug_monitor
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += /home/zos/robo/
INCLUDEPATH += /home/zos/robo/.whee/proto-gen/

LIBS += /home/zos/robo/.whee/build/native/ladybug/ladybug_state.a
LIBS += /home/zos/robo/.whee/build/native/ladybug/ladybug_client.a
LIBS += /home/zos/robo/.whee/build/native/network/socket.a
LIBS += /home/zos/robo/.whee/build/native/whee/source_root_sentinal.a
LIBS += /usr/local/lib/libprotobuf.a

QMAKE_CXXFLAGS += -include /home/zos/robo/primitives.h -std=gnu++14
