if(MSVC)
	# visual studio compile flags

	set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /arch:SSE2 /RTC1 /W3 /GR-")
	set(CMAKE_CXX_FLAGS_RELEASE "/W3 /MP /O2 /arch:SSE2 /GR- /MD")
else()

	#MinGW and g++ compile flags
	set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -fno-rtti -pipe")
	set(CMAKE_CXX_FLAGS_DEBUG "-Wall -std=c++11 -g -fno-rtti -pipe")
	set(CMAKE_CXX_FLAGS_RELEASE "-Wall -std=c++11 -O3 -fno-rtti -pipe")

	if(UNIX)

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC")

	endif(UNIX)

endif()
