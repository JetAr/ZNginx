#include <stdio.h>
#include <tchar.h>
#include <string.h>

#include "stdlib.h"
#include <ctime>
#include <errno.h>
#include <limits>


extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#define  BITS_PER_WORD ( CHAR_BIT*sizeof(unsigned int) )
#define  I_WORD(i) ((unsigned int)(i)/BITS_PER_WORD)
#define  I_BIT(i) (1 << (unsigned int)(i)% BITS_PER_WORD)
#define  checkarray(L) (NumArray*)luaL_checkudata(L , 1 , "LuaBook.array")



typedef struct NumArray
{
	int size;
	unsigned int values[1];
}NumArray;

#pragma warning(push)
#pragma warning(disable:4018)
static int newarray(lua_State* L)
{
	int n;
	size_t nbytes;
	NumArray *a;

	n = luaL_checkint(L , 1);
	luaL_argcheck(L , n>=1 , 1 , "invalid size");
	nbytes = sizeof(NumArray) + I_WORD(n-1) *sizeof(unsigned int);
	a = (NumArray*)lua_newuserdata(L , nbytes );

	a->size = n;
	for (int i =0 ; i<=I_WORD(n-1) ; ++i)
	{
		a->values[i] = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	luaL_getmetatable(L , "LuaBook.array");
	lua_setmetatable(L , -2);

	return 1;
}
#pragma warning(pop)

static unsigned int *getindex(lua_State* L , unsigned int *mask)
{
	NumArray* a = checkarray(L);
	int index = luaL_checkint(L , 2) -1;

	luaL_argcheck(L , 0<=index && index< a->size , 2 ,"index out of range");

	*mask = I_BIT(index);
	return &a->values[I_WORD(index)];
}

static int setarray(lua_State* L)
{
	unsigned int mask;
	unsigned int *entry = getindex(L , &mask);
	luaL_checkany(L , 3);
	if (lua_toboolean(L ,3))
		*entry |=mask;
	else
		*entry&=~mask;

	return 0;
}

static int getarray(lua_State* L)
{
	unsigned int mask;
	unsigned int *entry = getindex(L , &mask);
	lua_pushboolean(L , *entry &mask);
	
	return 1;
}

static int getsize(lua_State* L)
{
	NumArray* a = checkarray(L);
	lua_pushinteger(L , a->size);
	return 1;
}

static int array2string(lua_State*L)
{
	NumArray* a = checkarray(L);
	lua_pushfstring(L , "array(%d)" , a->size );
	return 1;
}

static const struct luaL_Reg arrayLib_f [] = 
{
	{"new" , newarray	},
	{NULL ,  NULL		}
};

static const struct luaL_Reg arrayLib_m [] =
{
	{"set" , setarray	},
	{"get" , getarray	},
	{"size", getsize	},
	{"__tostring", array2string	},
	{NULL ,  NULL		}
};

int luaopen_arry(lua_State* L)
{
	luaL_newmetatable(L , "LuaBook.array");
	lua_pushvalue(L ,-1);
	lua_setfield(L , -2 , "__index");
	
	luaL_register(L , NULL  , arrayLib_m);
	luaL_register(L , "array" , arrayLib_f );
	return 1;
}

//////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	//! test
	int bitsperword = BITS_PER_WORD;
	//printf("%d" , bitsperword);

	int error;
	// 创建Lua接口指针
	lua_State* L = lua_open();	
	// 加载Lua基本库
	//luaopen_base(L);    
	// 加载Lua通用扩展库
	luaL_openlibs(L);  

	//lua_pushcfunction(L , l_dir );
	//lua_setglobal(L , "dir");
	luaopen_arry(L);

	error = luaL_dofile(L, "array3.lua");



	lua_close(L);

	system("PAUSE");
	return 0;
}

