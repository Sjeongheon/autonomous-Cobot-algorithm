#include "amLua.hpp"

using namespace SYE;

YeLua::YeLua() {
}

////////////////////////////////////////////////////////////////////////////////////////////////////

YeLuaManager::YeLuaManager() {
	pLuaState = luaL_newstate();
}

YeLuaManager::~YeLuaManager() {
	lua_close(pLuaState);
}
