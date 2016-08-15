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
	// ����Lua�ӿ�ָ��
	lua_State* L = lua_open();	
	// ����Lua������
	luaopen_base(L);    
	// ����Luaͨ����չ��
	luaL_openlibs(L);     

	/* load the script */
	error = luaL_dofile(L, "test.lua");    // ��ȡLuaԴ�ļ����ڴ����

	lua_close( L);
	return 0;
}

