function(SetCompileFlags) 
	if(MSVC)
		# visual studio compile flags
		
		set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /arch:SSE2 /RTC1 /W3 /MP /GR-")
		set(CMAKE_CXX_FLAGS_RELEASE "/W3 /MP /O2 /arch:SSE2 /GR- /MD")
	else()

		#g++ compile flags
		set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -fno-rtti -fPIC -pipe")
		set(CMAKE_CXX_FLAGS_DEBUG "-Wall -std=c++11 -g -fno-rtti -fPIC -pipe")
		set(CMAKE_CXX_FLAGS_RELEASE "-Wall -std=c++11 -O2 -fno-rtti -fPIC -pipe")

	endif()
endfunction(SetCompileFlags)