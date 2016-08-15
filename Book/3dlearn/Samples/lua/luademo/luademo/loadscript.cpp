#include <stdio.h>
#include <tchar.h>

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

//--test.lua
//function add(x,y)
//return x+y
//end

//print("1+2" , add(1,2))

int _tmain(int argc, _TCHAR* argv[])
{
	lua_State* L = lua_open();	
	luaopen_base(L);    
	/* load the script */
	luaL_dofile(L, "test.lua");    // 读取Lua源文件到内存编译
	lua_close( L);
	return 0;
}

