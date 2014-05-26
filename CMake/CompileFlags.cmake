if(MSVC)
	# visual studio compile flags

	set(CMAKE_CXX_FLAGS_DEBUG "/W3 /DDEBUG_BUILD /MDd /Zi /Od /RTC1 /GR- /MP")
	set(CMAKE_CXX_FLAGS_RELEASE "/W3 /MD /O2 /GR- /MP")
	
else()

	#MinGW and g++ compile flags
        set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -fno-rtti -pipe")
        set(CMAKE_CXX_FLAGS_DEBUG "-Wall -std=c++0x -g -fno-rtti -pipe")
        set(CMAKE_CXX_FLAGS_RELEASE "-Wall -std=c++0x -O2 -fno-rtti -pipe")

	if(UNIX)

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC")

	endif(UNIX)
	
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_definitions(-DDEBUG_BUILD)
	endif()

endif()
