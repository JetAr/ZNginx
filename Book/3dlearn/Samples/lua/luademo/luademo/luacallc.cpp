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

//��lua���õķ���
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
	// ����Lua�ӿ�ָ��
	lua_State* L = lua_open();	
	// ����Lua������
	luaopen_base(L);    
	// ����Luaͨ����չ��
	luaL_openlibs(L);     

	lua_register(L, "average", average);
	/* load the script */
	error = luaL_dofile(L, "luacallc.lua");    // ��ȡLuaԴ�ļ����ڴ����

	lua_close( L);
	return 0;
}

