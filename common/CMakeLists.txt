

set(COMMON_HEADERS
    Camera.h
    RefCounting.h
    VecMath.h
    Timer.h
    Log.h
    CommonExport.h)

set(COMMON_SOURCE
    Camera.cpp
    RefCounting.cpp
    VecMath.cpp
    Timer.cpp
    Log.cpp)

# build the game engine
add_library(common SHARED ${COMMON_HEADERS} ${COMMON_SOURCE})
add_definitions(-DCOMMON_EXPORT)
