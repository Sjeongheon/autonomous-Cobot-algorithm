#ifndef AMLUA_HPP
#define AMLUA_HPP

#include <string>

namespace SYE {

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct _Lua {
	std::string file;
	std::string script;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class YeLua: private _Lua {
private:
protected:
public:
	YeLua();
	virtual ~YeLua() = default;

};

////////////////////////////////////////////////////////////////////////////////////////////////////

class YeLuaManager {
private:
	lua_State *pLuaState;
protected:
public:
	YeLuaManager();
	virtual ~YeLuaManager();
};

}

#endif
