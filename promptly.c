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

typedef struct fmt_t {
    const char *key;
    short       val;
} fmt_t;

fmt_t formats[] = {
    { "reset",      0 }, { "bright",   1 }, { "dim",        2 },
    { "underscore", 4 }, { "blink",    5 }, { "reverse",    7 },
    { "hidden",     8 }, { "black",   30 }, { "red",       31 },
    { "green",     32 }, { "yellow",  33 }, { "blue",      34 },
    { "magenta",   35 }, { "cyan",    36 }, { "white",     37 },
    { "onblack",   40 }, { "onred",   41 }, { "ongreen",   42 },
    { "onyellow",  43 }, { "onblue",  44 }, { "onmagenta", 45 },
    { "oncyan",    46 }, { "onwhite", 47 }, { NULL, 0 }
};

static int l_traceback(lua_State* L) {
    const char *msg = lua_tostring(L, 1);
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
            if (lua_isnil(L, -1))
                goto skip;
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

inline void env_add(lua_State *L, const char* k, const char* v) {
    lua_pushstring(L, v);
    lua_setfield(L, -2, k);
}

int main(int argc, const char* argv[]) {
    int   i;
    char  hostname[16];
    char *realhome = NULL;
    const char *confpath, *home, *pwd, *shortpwd, *termname, *username, *err;
    confpath = pwd = shortpwd = termname = err = NULL;

    if (!(pwd = getcwd(NULL, 0)))  
        ERROR("pwd");
    if (!(home = getenv("HOME")))  
        ERROR("home");
    if (!(realhome = realpath(home, realhome))) 
        ERROR("realpath");
    if (!((username = getenv("USER")) || (username = getenv("LOGNAME"))))
        ERROR("username");
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
    for (i = 0; formats[i].key != NULL; i++) {
        lua_pushstring(L, formats[i].key);
        lua_pushnumber(L, formats[i].val);
        lua_settable(L, -3);
    }
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

