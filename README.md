# promptly

promptly is a shell prompt which has had a Lua put in it.

## Installation

- Have pkg-config and LuaJIT or Lua 5.2 installed
- Run `make LUA=xyz`, replacing `xyz` with your LuaJIT or Lua 5.2 pkg-config name
- Put `promptly` somewhere on your PATH
- Add `PS1='$(promptly $?)'` to your `.bashrc`, or the config file of whatever dumb esoteric shell you use while picking pipe tobacco out of your beard

If you don't know the right pkg-config name for your distro, run `pkg-config --list-all | grep lua`.

## Usage

`promptly` expects to find a lua script at `$HOME/.promptly`. If there isn't one, `promptly` will complain, and default to a very dull prompt.

The script has a global table `env` set. Initially it contains these values:

- `PWD`    the current working directory
- `USER`   your username
- `HOST`   the machine's hostname
- `PWD`    the working directory
- `TTY`    the current tty name
- `STATUS` status of the last executed command if non-zero

If the script returns a string it will be used as the prompt, interpolated with the values from `env` like so:`

    local time = os.date("%H:%M:%S")
    env.TIME = time
    
    return "[$TIME] ($USER@$HOST) $PWD $ "

If the script doesn't return a string, it's assumed you wrote out your own prompt (as below), and nothing else is printed.

    io.write( "(", env.USER, "@", env.HOST, ")", env.PWD, " $ " )

## Colours and formatting

Use `fmt()` for text formatting. The following examples both print "hello there" in red:

    print( fmt("red") .. "hello there" .. fmt("reset") )
    print( fmt("red", "hello there") )  -- automatically resets

More than one format may be applied at once:

    print( fmt("red", "bright", "hello there") )

The available formats are:

- Formatting: **reset**, **bright**, **dim**, **underscore**, **blink**, **reverse**, **hidden**
- Foreground colours: **black**, **red**, **green**, **yellow**, **blue**, **magenta**, **cyan**, **white**
- Background colours: **onblack**, **onred**, **ongreen**, **onyellow**, **onblue**, **onmagenta**, **oncyan**, **onwhite**
- xterm foreground colours: a number (0-256)

## Known bugs

Lines will wrap too early when using formatting. Non-printable characters seem to be correctly escaped, and the prompt otherwise behaves correctly in this regard, so I'm pretty much stumped on this one.

## What, no Lua 5.1?

If you explicitly need Lua 5.1 support (and I really can't imagine why you would) you'll need to implement [luaL_traceback](https://github.com/hishamhm/lua-compat-5.2/blob/master/c-api/compat-5.2.c#L228) yourself.

## License

See LICENSE.

