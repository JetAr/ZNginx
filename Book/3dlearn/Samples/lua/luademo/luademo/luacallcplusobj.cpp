#include <stdio.h>
#include <tchar.h>

#include <iostream>
using namespace std;

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

//--luacallcplusfun.lua
//
//o = obj();
//o:set( 50 );
//o:get();
//print("lua got average value:" , avg)

class obj
{
public:
	obj() : val( 0 )
	{
		int i = 0 ;
	}

	void   set( double v )
	{ 
		val = v;
	}
	double get( void )  
	{ 
		return val;
	}

private:
	double val;
};

class lua_bind
{
public:
	static void reg( lua_State* L )
	{
		lua_newtable( L );
		int tbl = lua_gettop( L );

		luaL_newmetatable( L, "obj" );
		int meta_tbl = lua_gettop( L );

		lua_pushliteral( L, "__metatable" );
		lua_pushvalue( L, tbl );
		lua_settable( L, meta_tbl );

		lua_pushliteral( L, "__index" );
		lua_pushvalue( L, tbl );
		lua_settable( L, meta_tbl );

		lua_pushliteral( L, "__gc" );
		lua_pushcfunction( L, gc );
		lua_settable( L, meta_tbl );

		lua_pop( L, 1 );
		luaI_openlib( L, 0, functions, 0 );

		lua_pop( L, 1 );
		lua_register( L, class_name, build );
	}

	static int build( lua_State* lua )
	{
		obj* p = new obj();
		*( void** )( lua_newuserdata( lua, sizeof( void* ) ) ) = p;

		luaL_getmetatable( lua, class_name );
		lua_setmetatable( lua, -2 );

		cout << "build object, val is 0" << endl;
		return 1;
	}

	static int gc( lua_State* lua )
	{
		obj* p = ( obj* )( *( void** )( lua_touserdata( lua, 1 ) ) );
		delete p;

		cout << "object gc" << endl;
		return 0;
	}

	static int set_val( lua_State* lua )
	{
		obj* p = ptr( lua, 1 );
		double val = luaL_checknumber( lua, 2 );

		p->set( val );

		cout << "set value to " << val << endl;
		return 0;
	}

	static int get_val( lua_State* lua )
	{
		obj* p = ptr( lua, 1 );
		double val = p->get();

		lua_pushnumber( lua, val );

		cout << "get value is " << val << endl;
		return 1;
	}

	static obj* ptr( lua_State* lua, int narg )
	{
		luaL_checktype( lua, narg, LUA_TUSERDATA );
		void* ud = luaL_checkudata( lua, narg, class_name );

		if( ud )
			return *( obj** )ud;

		luaL_typerror( lua, narg, class_name );
		return 0;
	}

	static const char     class_name[];
	static const luaL_reg functions[];
};

const char lua_bind::class_name[] = "obj";

const luaL_reg lua_bind::functions[] =
{
	{ "set", lua_bind::set_val },
	{ "get", lua_bind::get_val },
	{     0,                 0 }
};

int _tmain(int argc, _TCHAR* argv[])
{
	lua_State* L = lua_open();	

	lua_bind::reg( L );

	luaL_dofile( L, "luacallcplusobj.lua" );

	lua_close( L);

	return 0;
}

