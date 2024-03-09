#ifndef AMLUA_HPP
#define AMLUA_HPP

#include <string>
#include <map>

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
	YeLua(const char *);
	virtual ~YeLua() = default;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class YeLuaManager: public std::map<std::string, YeLua *>  {
private:
	std::string mPath;
	lua_State *pLuaState;
protected:
public:
	YeLuaManager(const char *);
	virtual ~YeLuaManager();
};

}

#endif
