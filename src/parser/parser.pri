# Parser

HEADERS += $$SRC_DIR/parser/entitylist.h      \
           $$SRC_DIR/parser/entity.h          \
           $$SRC_DIR/parser/attributelist.h   \
           $$SRC_DIR/parser/attribute.h       \
           $$SRC_DIR/parser/masked_isspace.h  \
           $$SRC_DIR/parser/defs.h

SOURCES += $$SRC_DIR/parser/entitylist.cpp    \
           $$SRC_DIR/parser/entity.cpp        \
           $$SRC_DIR/parser/attributelist.cpp \
           $$SRC_DIR/parser/attribute.cpp     \
           $$SRC_DIR/parser/masked_isspace.cpp
