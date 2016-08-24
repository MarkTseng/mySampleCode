/*
* Copyright (c) 2011 Mark Tseng
* test c module in lua
*/

#include <lua.h>
#include <lauxlib.h>

#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

static int l_dir(lua_State *L)
{
	printf("call %s\n",__func__);
	DIR *dir;
	struct dirent *entry;
	int i;

	// check 1st argument is string type
	const char *path = luaL_checkstring(L,1);

	/* open dir*/

	dir = opendir(path);
	if(dir == NULL)
	{
		lua_pushnil(L);
		lua_pushstring(L,strerror(errno));
		return 2; /*number of result*/
	}

	/* create a result table*/

	lua_newtable(L);
	i=1;
	while((entry = readdir(dir)) != NULL)
	{
		lua_pushnumber(L,i++);   // stack index is -1 in Lua_State
		lua_pushstring(L,entry->d_name); 
		lua_settable(L,-3);
	}

	closedir(dir);

	return 1;
}

static const struct luaL_Reg mylib [] = {
	{"dir",l_dir},
	{NULL,NULL}
};


// for rquire("mylib"), then it will register mylib Reg function call
int luaopen_mylib(lua_State *L)
{
	luaL_register(L,"mylib", mylib);
	printf("open %s\n",__func__);
	return 1;
}
