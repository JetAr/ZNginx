#include <stdio.h>
#include <tchar.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
//--luacallc.lua
//avg = average(20 , 30 , 4)
//print("lua got average value:" , avg)

//被lua调用的方法
static int average(lua_State * L)
{
	/* get number of arguments */
	int n = lua_gettop(L);
	int sum=0;
	/* loop through each argument */
	for (int i = 1; i <= n; i++)
	{
		/* total the arguments */
		sum += lua_tonumber(L, i);
	}

	lua_pushnumber(L, sum / n);
	/* return the number of results */
	printf("c average called. [ok]\n");
	return 1;
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

	lua_register(L, "average", average);
	/* load the script */
	error = luaL_dofile(L, "luacallc.lua");    // 读取Lua源文件到内存编译

	lua_close( L);
	return 0;
}

