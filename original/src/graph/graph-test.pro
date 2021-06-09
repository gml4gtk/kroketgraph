
TEMPLATE = app
LANGUAGE = C++

SRC_DIR = ..

include ( ../parser/parser.pri )
include ( graph.pri )

SOURCES += graph-test.cpp

TARGET = graph-test

CONFIG += warn_on debug

#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg
