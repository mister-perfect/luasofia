/* vim: set ts=8 et sw=4 sta ai cin: */
/*
 * @author Paulo Pizarro  <paulo.pizarro@gmail.com>
 * @author Tiago Katcipis <tiagokatcipis@gmail.com>
 *
 * This file is part of Luasofia.
 *
 * Luasofia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Luasofia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Luasofia.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include "utils/luasofia_userdata_table.h"

static int userdata_table_ref = LUA_REFNIL;

void luasofia_userdata_table_remove(lua_State *L, void* key)
{
    /* put the userdata table at the stack */
    lua_rawgeti(L, LUA_REGISTRYINDEX, userdata_table_ref);
    if (lua_isnil(L, -1))
        luaL_error(L, "Failed to get userdata table!");

    /* userdata_table[key] = nil */
    lua_pushlightuserdata(L, key);
    lua_pushnil(L);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

void luasofia_userdata_table_get(lua_State *L, void* key)
{
    /* put the userdata table at the stack */
    lua_rawgeti(L, LUA_REGISTRYINDEX, userdata_table_ref);
    if (lua_isnil(L, -1))
        luaL_error(L, "Failed to get userdata table!");

    /* get userdata_table[key] */
    lua_pushlightuserdata(L, key);
    lua_rawget(L, -2);

    /* remove the table from the stack */
    lua_remove(L, -2);
}

void luasofia_userdata_table_set(lua_State *L, void* key)
{
   /* put the userdata table at the stack */
    lua_rawgeti(L, LUA_REGISTRYINDEX, userdata_table_ref);
    if (lua_isnil(L, -1))
        luaL_error(L, "Failed to get userdata table!");

    /* put key at top of the stack */
    lua_pushlightuserdata(L, key);

    /* put userdata at top of the stack */
    lua_pushvalue(L, -3);

    /* userdata_table[key] = userdata */
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

void luasofia_userdata_table_create(lua_State *L)
{
    if(userdata_table_ref != LUA_REFNIL)
        return;

    /* create userdata table */
    lua_newtable(L);
   
    /* now lets store the userdata table at the LUA_REGISTRYINDEX, */
    /* so it can be acessed by the unregistered callback functions */
    userdata_table_ref = luaL_ref(L, LUA_REGISTRYINDEX);
}

