#include <stdio.h>
#include <tchar.h>

#include "stdlib.h"
#include <ctime>

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

//--hellow.lua
//function add(x,y)
//print("x+y=" , x+y );
//return
//end
//
//--print(dddd)
//add(1 , 2)

double fun(lua_State* L ,  double x, double y )
{
	double ret;
	lua_getglobal( L, "add");      
	lua_pushnumber( L,x);         
	lua_pushnumber( L,y);         
	lua_call( L, 2, 1);          
	ret = lua_tonumber( L, -1);    
	lua_pop( L, 1);              
	return ret;
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
	error = luaL_dofile(L, "test.lua");

	srand( time(0) );
	while(1)
	{
		int alpha = rand();
		int beta = rand();
		int ret = fun(L , alpha , beta );
		printf("%d " , ret);
	}

	lua_close( L);
	return 0;
}

