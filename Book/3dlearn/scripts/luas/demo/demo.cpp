// demo.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

//#include <lua.h>
//#include <lualib.h>
//#include <lauxlib.h>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


int main(int argc, char* argv[])
{
	int error;
	// 创建Lua接口指针
	lua_State* L = luaL_newstate();	
	// 加载Lua基本库
	//luaopen_base(L);    
	// 加载Lua通用扩展库
	luaL_openlibs(L);     

	char buff[256];
	int len=0;

#if 1
	sprintf_s(buff , 256 , "x=3;y=17;z=x+y;print(z)");
	len = strlen(buff );
	error = luaL_loadbuffer(L , buff ,  len , "line");
	//{
	//	LoadS ls;
	//	ls.s = buff;
	//	ls.size = size;
	//	error = lua_load(L, getS, &ls, name, mode);

	//}
	if (error)
	{
		fprintf(stderr , "%s" , lua_tostring(L , -1) );
		lua_pop(L , 1);
	}

	error = lua_pcall(L , 0,0,0);
	if (error)
	{
		fprintf(stderr , "%s" , lua_tostring(L , -1) );
		lua_pop(L , 1);
	}


	system("pause");

#else
	while (fgets(buff , sizeof(buff) , stdin) != NULL )
	{
		//int n = lua_gettop(L);
		error = luaL_loadbuffer(L , buff , strlen(buff )  , "line") || lua_pcall(L , 0,0,0);
		//fprintf(stderr , "%s" , lua_tostring(L , -1) );
		//n = lua_gettop(L);
		if (error)
		{
			fprintf(stderr , "%s" , lua_tostring(L , -1) );
			lua_pop(L , 1);
		}
	}

#endif



	lua_close(L);

	return 0;
}

