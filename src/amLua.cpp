#include "amLua.hpp"

using namespace SYE;

YeLua::YeLua(const char *scriptFileName) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////

YeLuaManager::YeLuaManager(const char *scriptPath) {
	pLuaState = luaL_newstate();
}

YeLuaManager::~YeLuaManager() {
	lua_close(pLuaState);
}
