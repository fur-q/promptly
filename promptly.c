#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define ERROR(e)    do { err = e; goto error; } while (0)
#define XTERM_COLOR "\x1b[38;5;%dm%s\x1b[0m"
#define ANSI_COLOR  "\x1b[%dm\x1b[0m"
#define CONFNAME    ".promptly"

static int traceback(lua_State* L) {
    const char *msg;
    msg = lua_tostring(L, 1);
    if (!msg) return 0;
    luaL_traceback(L, L, msg, 1);
    return 1;
}

inline void env_add(lua_State *L, const char* k, const char* v) {
    lua_pushstring(L, v);
    lua_setfield(L, -2, k);
}

inline void fmt_add(lua_State *L, const char* k, int v) {
    lua_pushstring(L, k);
    lua_pushnumber(L, v);
    lua_settable(L, -3);
}

inline void formatting(lua_State *L) {
    fmt_add(L, "reset", 0);
    fmt_add(L, "bright", 1);
    fmt_add(L, "dim", 2);
    fmt_add(L, "underscore", 4);
    fmt_add(L, "blink", 5);
    fmt_add(L, "reverse", 7);
    fmt_add(L, "hidden", 8);
    fmt_add(L, "black", 30);
    fmt_add(L, "red", 31);
    fmt_add(L, "green", 32);
    fmt_add(L, "yellow", 33);
    fmt_add(L, "blue", 34);
    fmt_add(L, "magenta", 35);
    fmt_add(L, "cyan", 36);
    fmt_add(L, "white", 37);
    fmt_add(L, "onblack", 40);
    fmt_add(L, "onred", 41);
    fmt_add(L, "ongreen", 42);
    fmt_add(L, "onyellow", 43);
    fmt_add(L, "onblue", 44);
    fmt_add(L, "onmagenta", 45);
    fmt_add(L, "oncyan", 46);
    fmt_add(L, "onwhite", 47);
}

static int l_fmt(lua_State *L) {
    const char* str = lua_tostring(L, 1);
    if (lua_isnumber(L, 2))
        lua_pushfstring(L, XTERM_COLOR, lua_tointeger(L, 2), str);
    else {
        lua_gettable(L, lua_upvalueindex(1));
        if (!lua_isnumber(L, -1)) {
            lua_pushvalue(L, 1);
        } else
            lua_pushfstring(L, ANSI_COLOR, lua_tointeger(L, -1), str);
    }
    return 1;
}

int main(int argc, const char* argv[]) {
    char  hostname[16];  // ?
    const char* confpath = NULL;
    const char* home     = NULL;
    const char* realhome = NULL;
    const char* pwd      = NULL;
    const char* shortpwd = NULL;
    const char* termname = NULL;
    const char* username = NULL;
    const char* err      = NULL;

    if (!(pwd = getcwd(NULL, 0)))
        ERROR("Error getting working directory");
    if (!(home = getenv("HOME")))
        ERROR("Error getting home directory");
    if (!(realhome = realpath(home, realhome)))
        ERROR("Error resolving home directory");
    if (!((username = getenv("USER")) || (username = getenv("LOGNAME"))))
        ERROR( "Error getting username");
    if (!(termname = ttyname(0)))
        ERROR("Error getting tty name");
    if (gethostname(hostname, 15))
        ERROR("Error getting hostname");
    hostname[15] = '\0';

    lua_State* L = luaL_newstate();
    if (!L)
        ERROR("Error initialising Lua state");
    luaL_openlibs(L);
    lua_pushcfunction(L, traceback);

    confpath = lua_pushfstring(L, "%s/%s", home, CONFNAME);
    if ((luaL_loadfile(L, confpath)))
        ERROR(lua_tostring(L, -1));

    shortpwd = luaL_gsub(L, pwd, realhome, "~");
    lua_newtable(L);
    env_add(L, "PWD", shortpwd);
    env_add(L, "HOME", home);
    env_add(L, "USER", username);
    env_add(L, "HOST", hostname);
    env_add(L, "TTY", termname);
    if (argc > 1 && strcmp(argv[1], "0"))
        env_add(L, "STATUS", argv[1]);
    lua_setglobal(L, "env");
    lua_pop(L, 1);   // shortpwd

    lua_newtable(L);
    formatting(L);
    lua_pushcclosure(L, l_fmt, 1);
    lua_setglobal(L, "fmt");

    if (lua_pcall(L, 0, 1, 1))
        ERROR(lua_tostring(L, -1));
    if (!lua_isstring(L, 3))
        return 0;

    lua_getglobal(L, "string");
    lua_getfield(L, -1, "gsub");
    lua_pushvalue(L, 3);
    lua_pushliteral(L, "%$(%u+)");
    lua_getglobal(L, "env");
    if (lua_pcall(L, 3, 1, 1))
        ERROR(lua_tostring(L, -1));
    printf("%s", lua_tostring(L, -1));

    lua_close(L);
    return 0;

error:
    
    printf("Error: %s\n$ ", err);
    return 1;

}
