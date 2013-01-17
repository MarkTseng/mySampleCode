/*
* Copyright (c) 2011 Mark Tseng
* test c module in lua with userdata
*/

#include <lua.h>
#include <lauxlib.h>

#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>

#define BITS_PER_WORD (CHAR_BIT*sizeof(unsigned int))
#define I_WORD(i)	((unsigned)(i) / BITS_PER_WORD)
#define I_BIT(i)	(1 << ((unsigned)(i) % BITS_PER_WORD))

typedef struct NumArray {
	int size;
	unsigned int values[1]; /* variable part*/
} NumArray;

/* new(int) in lua  */
static int newarray(lua_State *L)
{
	int i,n;
	size_t nbytes;
	NumArray *a;

	n = luaL_checkint(L,1);

	/* check 1st argument is integer and bigger than 1
	 * or it will show error message;
	 * */
	luaL_argcheck(L, n >= 1, 1, "invalid size");

	// call lua_newuserdata and give size of structure
	nbytes = sizeof(NumArray) + I_WORD(n -1)*sizeof(unsigned int);
	a = (NumArray *)lua_newuserdata(L,nbytes);
	a->size = n;
	for(i=0 ; i <= I_WORD(n-1) ; i++)
	{
		a->values[i] = 0; /* initial array */
	}

	return 1;
}

static int setarray(lua_State *L)
{
	NumArray *a = (NumArray *)lua_touserdata(L,1);
	int index = luaL_checkint(L,2) - 1;

	luaL_checkany(L,3);

	luaL_argcheck(L, a != NULL, 1, "'array' expected");

	luaL_argcheck(L, 0 <= index, 2, "index out of range");

	if(lua_toboolean(L,3))
	{
		a->values[I_WORD(index)] |= I_BIT(index);
	}
	else
	{
		a->values[I_WORD(index)] &= ~I_BIT(index);
	}

	return 0;
}

static int getarray(lua_State *L)
{
	NumArray *a = (NumArray *)lua_touserdata(L,1);
	int index = luaL_checkint(L,2) - 1;

	luaL_argcheck(L, a != NULL, 1, "'array' expected");

	luaL_argcheck(L, 0 <= index && index < a->size, 2, "index out of range");

	lua_pushboolean(L,a->values[I_WORD(index)] & I_BIT(index));
	return 1;
}

static int getsize(lua_State *L)
{
	printf("call %s\n",__func__);
	NumArray *a = (NumArray *)lua_touserdata(L,1);
	luaL_argcheck(L, a != NULL, 1, "'array' expected");

	lua_pushinteger(L, a->size);
	return 1;
}

static const struct luaL_Reg arraylib [] = {
	{"new",newarray},
	{"set",setarray},
	{"get",getarray},
	{"size",getsize},
	{NULL,NULL}
};


// for rquire("array"), then it will register mylib Reg function call
int luaopen_array(lua_State *L)
{
	luaL_register(L,"array", arraylib);
	printf("open %s\n",__func__);
	return 1;
}
