
TEMPLATE = app
LANGUAGE = C++

TARGET = kroket

VER_MAJ = 0
VER_MIN = 8
VER_PAT = 1

VERSION = -0.8.1

win32 {
DEFINES += MSWIN
}

DEFINES += KROKET_VER_MAJ=$$VER_MAJ KROKET_VER_MIN=$$VER_MIN KROKET_VER_PAT=$$VER_PAT

CONFIG += qt warn_on release

SRC_DIR = src

include ( src/application.pri )

FORMS = ui/kroket.ui ui/help.ui ui/help-input.ui ui/help-about.ui
RESOURCES = ui/kroket.qrc
INCLUDEPATH = src
OBJECTS_DIR = build
UI_DIR = build
MOC_DIR = build
RCC_DIR = build
