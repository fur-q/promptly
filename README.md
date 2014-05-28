# promptly

promptly is a shell prompt which has had a Lua put in it.

## Installation

- Have pkg-config and LuaJIT or Lua 5.2 installed
- Figure out what your distro has decided to call your LuaJIT or Lua 5.2 pkg-config package (probably `luajit`, `lua5.2` or `lua-5.2`)
- Run `make LUA=myluapkgconfigname`
- Put `promptly` somewhere on your PATH
- Add `PS1='$(promptly $?)'` to your `.bashrc`, or the config file of whatever dumb esoteric shell you use while picking pipe tobacco out of your beard

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

## License

See LICENSE.

