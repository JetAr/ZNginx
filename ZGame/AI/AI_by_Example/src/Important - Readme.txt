March 2008

----------

This release resolves the handful of bugs found since 2004 (see http://www.ai-junkie.com/ai_book2/bugs/index.html for details) but mainly fixes the problems related to newer versions of Visual Studio, Lua and Luabind.

I haven't been able to check this code with every version of Visual Studio so plz let me know if you experience problems other than the ones described in this file. My email is fup@ai-junkie.com. Thanks.



Visual Studio 6 - 7 Users
-------------------------

Use the project files found in the "VC6 Projects" folder. If you are using VS7 then click on the relevent workspace file (dsw) and your compiler should convert it to a VS solution file without any problems.

Note that the Luabind scripting projects now utilize a prebuilt Luabind lib instead of the source files. This means you'll have to add the Luabind lib path to your dev environment.

Make sure you use Boost 1.32.00, which you can find here: http://sourceforge.net/project/showfiles.php?group_id=7586




VS8 Users
---------

Use the project files found in the "VC8 Projects" folder.

These contain fixes allowing the code to be compiled using the lastest version of Visual Studio. The majority are related to updates of Lua and Luabind.

This includes:

	The use of Lua5.1.3 instead of Lua5.0 (Lua5.0 will not compile in VS8).

Note that:

	lua5.1.lib replaces lua.lib and lualib.lib
	instances of "lua_dofile" have been changed to the Lua5.1 "luaL_dofile".
	also the new call to load the Lua libaries "luaL_openlibs" is utilized instead of the helper method utilized in the original

Unfortunately Luabind does not currently support Lua5.1. However the latest SVN version of Luabind utilized by the projects ScriptedStateMachine & ExposingCPPFunctionsToLua will build and run in *release* mode but they produce errors when run in debug mode. For this reason I don't recommend the use of Luabind until the authors have sorted out the issues (the project CreatingClassesUsingLuabind & ExposingClassesUsingLuabind will compile and run okay in debug mode). **If you still want to compile the Luabind projects you will need to install the latest version of Boost (1.34.1 as I write this), found here: http://sourceforge.net/project/showfiles.php?group_id=7586



MS Windows Vista Problems
-------------------------

Someone wrote to me to mention they were having problems compiling the source under Vista. I dont use Vista so I couldn't help but they eventually solved the problem by changing the character set in program properties to “Not Set” and commenting out the lines

_DEBUG_ERROR("list iterators incompatible");
_SCL_SECURE_TRAITS_INVALID_ARGUMENT;

In the list standard header.

