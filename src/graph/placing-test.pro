
TEMPLATE = app
LANGUAGE = C++

SRC_DIR = ..

include ( ../parser/parser.pri )
include ( graph.pri )

SOURCES += placing-test.cpp

TARGET = placing-test

CONFIG += warn_on release

DEFINES += PLACING_TEST

#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg
