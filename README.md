# promptly

promptly is a shell prompt which has had a Lua put in it.

## Installation

- Have pkg-config and LuaJIT or Lua 5.2 installed
- Run `make` to build with LuaJIT, or `make LUA=lua-5.2` to build with Lua 5.2
- Put `promptly` somewhere on your PATH
- Put `PS1='$(promptly $?)'` in your `.bashrc` or equivalent

## Usage

`promptly` expects to find a lua script at `$HOME/.promptly`. If there isn't one, `promptly` will complain and give you a very dull prompt.

The script has a global table `env` set. Initially it contains these values:

- `USER`   your username
- `HOST`   the machine's hostname
- `PWD`    the working directory
- `TTY`    the current tty name
- `STATUS` status of the last executed command if non-zero

If the script returns a string it will be used as the prompt, using values from the `env` table like so:

    return "($USER@$HOST) $PWD $ "

You can add your own values to `env`:

    local time = os.date("%H:%M:%S")
    env.TIME = time
    
    return "[$TIME] ($USER@$HOST) $PWD $ "

Alternatively, you can just write out your own prompt:

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

