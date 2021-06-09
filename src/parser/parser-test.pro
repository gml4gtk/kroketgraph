
TEMPLATE = app
LANGUAGE = C++

SRC_DIR = ..

include ( parser.pri )

SOURCES += parser-test.cpp

TARGET = parser-test

CONFIG += warn_on debug

#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg
