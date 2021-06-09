# Graph

HEADERS += $$SRC_DIR/graph/node.h              \
           $$SRC_DIR/graph/edge.h              \
           $$SRC_DIR/graph/graph.h             \
           $$SRC_DIR/graph/layering-lazy.h     \
           $$SRC_DIR/graph/layering-floyd.h    \
           $$SRC_DIR/graph/ordering-wmedian.h  \
           $$SRC_DIR/graph/placing-genetic.h   \
           $$SRC_DIR/graph/placing-cuckoo.h

SOURCES += $$SRC_DIR/graph/node.cpp              \
           $$SRC_DIR/graph/edge.cpp              \
           $$SRC_DIR/graph/graph.cpp             \
           $$SRC_DIR/graph/layering-lazy.cpp     \
           $$SRC_DIR/graph/layering-floyd.cpp    \
           $$SRC_DIR/graph/ordering-wmedian.cpp  \
           $$SRC_DIR/graph/placing-genetic.cpp   \
           $$SRC_DIR/graph/placing-cuckoo.cpp
