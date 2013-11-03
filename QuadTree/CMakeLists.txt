

set(QUADTREE_HEADERS QuadTree.h NodeIterator.h ISpatialObject.h)

set(QUADTREE_SOURCE QuadTree.cpp NodeIterator.cpp)

# build the QuadTree
add_library(QuadTree STATIC ${QUADTREE_HEADERS} ${QUADTREE_SOURCE})
target_link_libraries(QuadTree common)
