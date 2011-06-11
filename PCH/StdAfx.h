#ifndef _STDAFX_
#define _STDAFX_
#pragma once

#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>

// stl
#include <list>
#include <vector>
#include <map>
#include <string>
#include <sstream>

// DirextX
#include <d3d9.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800//
#include <dinput.h>

// boost
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>

// AngelScript
#include "angelscript.h"
#include "ScriptBuilder.h"
#include "scriptstdstring.h"
#include "scripthelper.h"

#endif // _STDAFX_