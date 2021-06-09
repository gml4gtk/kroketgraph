# Application

include ( $$SRC_DIR/parser/parser.pri )
include ( $$SRC_DIR/graph/graph.pri )
include ( $$SRC_DIR/anchorshapes/anchorshapes.pri )
include ( $$SRC_DIR/nodeshapes/nodeshapes.pri )
include ( $$SRC_DIR/undocommands/undocommands.pri )

HEADERS += $$SRC_DIR/application.h               \
           $$SRC_DIR/graphview.h                 \
           $$SRC_DIR/graphviewtab.h              \
           $$SRC_DIR/help.h                      \
           $$SRC_DIR/abstractshape.h             \
           $$SRC_DIR/port.h                      \
           $$SRC_DIR/abstractanchor.h            \
           $$SRC_DIR/abstractnodeshape.h         \
           $$SRC_DIR/abstractedgeshape.h         \
           $$SRC_DIR/edge-simple.h               \
           $$SRC_DIR/abstractgroupshape.h        \
           $$SRC_DIR/group-simple.h              \
           $$SRC_DIR/convexhull.h

SOURCES += $$SRC_DIR/application.cpp               \
           $$SRC_DIR/graphview.cpp                 \
           $$SRC_DIR/graphviewtab.cpp              \
           $$SRC_DIR/help.cpp                      \
           $$SRC_DIR/main.cpp                      \
           $$SRC_DIR/abstractshape.cpp             \
           $$SRC_DIR/port.cpp                      \
           $$SRC_DIR/abstractanchor.cpp            \
           $$SRC_DIR/abstractnodeshape.cpp         \
           $$SRC_DIR/abstractedgeshape.cpp         \
           $$SRC_DIR/edge-simple.cpp               \
           $$SRC_DIR/abstractgroupshape.cpp        \
           $$SRC_DIR/group-simple.cpp              \
           $$SRC_DIR/convexhull.cpp
