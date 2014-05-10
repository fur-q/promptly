#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define ERROR(e)    do { err = e; goto error; } while (0)
#define XTERM_COLOR "\001\x1b[38;5;%fm\002"
#define ANSI_COLOR  "\001\x1b[%fm\002"
#define ANSI_RESET  "\001\x1b[0m\002"
#define CONFNAME    ".promptly"

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

static int l_traceback(lua_State* L) {
    const char *msg;
    msg = lua_tostring(L, 1);
    if (!msg)
        return 0;
    luaL_traceback(L, L, msg, 1);
    return 1;
}

static int l_fmt(lua_State *L) {
    luaL_Buffer buf;
    const char *str = NULL;

    luaL_buffinit(L, &buf);
    if (lua_gettop(L) > 1) {
        str = lua_tostring(L, -1);
        lua_pop(L, 1);
    }
    while (lua_gettop(L) > 0) {
        if (lua_isnumber(L, -1)) {
            lua_pushfstring(L, XTERM_COLOR, lua_tonumber(L, -1));
        } else {
            lua_gettable(L, lua_upvalueindex(1));
            if (lua_isnil(L, -1)) {
                goto skip;
            }
            lua_pushfstring(L, ANSI_COLOR, lua_tonumber(L, -1));
        }
        luaL_addvalue(&buf);
skip:
        lua_pop(L, 1);
    }
    if (str != NULL) {
        luaL_addstring(&buf, str);
        luaL_addstring(&buf, ANSI_RESET);
    }
    luaL_pushresult(&buf);
    return 1;
}

int main(int argc, const char* argv[]) {
    char  hostname[16];
    char* realhome = NULL;
    const char *confpath, *home, *pwd, *shortpwd, *termname, *username, *err;
    confpath = pwd = shortpwd = termname = err = NULL;

    if (!(pwd = getcwd(NULL, 0)))  
        ERROR("pwd");
    if (!(home = getenv("HOME")))  
        ERROR("home");
    if (!(realhome = realpath(home, realhome))) 
        ERROR("realpath");
    if (!((username = getenv("USER")) || (username = getenv("LOGNAME"))))
        ERROR( "username");
    if (!(termname = ttyname(0)))  
        ERROR("termname");
    if (gethostname(hostname, 15)) 
        ERROR("hostname");

    hostname[15] = '\0';

    lua_State* L = luaL_newstate();
    if (!L)
        ERROR("luaL_newstate()");
    luaL_openlibs(L);
    lua_pushcfunction(L, l_traceback);

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
    lua_pop(L, 1);

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

