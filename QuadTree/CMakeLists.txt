

set(QUADTREE_HEADERS QuadTree.h)

set(QUADTREE_SOURCE QuadTree.cpp)

# build the QuadTree
add_library(QuadTree STATIC ${QUADTREE_HEADERS} ${QUADTREE_SOURCE})
target_link_libraries(QuadTree common)
