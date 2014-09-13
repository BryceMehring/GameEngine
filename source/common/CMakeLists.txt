

set(COMMON_HEADERS
    Camera.h
    VecMath.h
    Timer.h
    Log.h
	CommonExport.h
	Random.h)

set(COMMON_SOURCE
    Camera.cpp
    VecMath.cpp
    Timer.cpp
	Log.cpp
	Random.cpp)

# build the common shared lib
add_library(common SHARED ${COMMON_HEADERS} ${COMMON_SOURCE})
add_definitions(-DCOMMON_EXPORT)

if(ENABLE_CPACK)
install(TARGETS common
	RUNTIME DESTINATION ./ COMPONENT Runtime
	LIBRARY DESTINATION ./ COMPONENT Runtime
	)
endif(ENABLE_CPACK)
