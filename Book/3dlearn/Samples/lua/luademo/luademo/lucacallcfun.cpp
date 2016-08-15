// luademo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

int _tmain(int argc, _TCHAR* argv[])
{
	int error;
	// 创建Lua接口指针
	lua_State* L = lua_open();	
	// 加载Lua基本库
	luaopen_base(L);    
	// 加载Lua通用扩展库
	luaL_openlibs(L);     

	/* load the script */
	error = luaL_dofile(L, "test.lua");    // 读取Lua源文件到内存编译

	lua_close( L);
	return 0;
}

