# promptly

promptly is a shell prompt, except I put a Lua in it.

## Installation

Have pkg-config and LuaJIT installed and run `make`, then put `promptly` somewhere on your `$PATH`.

## Usage

`promptly` expects to find a lua script at `$HOME/.promptly`. If there isn't one, `promptly` will complain and give you a very dull prompt.

The script has a global table `env` set. Initially it contains these values:

`USER`   your username
`HOST`   the machine's hostname
`PWD`    the working directory
`TTY`    the current tty name
`STATUS` status of the last executed command if non-zero

If the script returns a string it will be used as the prompt, using values from the `env` table like so:

    return "($USER@$HOST) $PWD $ "

You can add your own values to `env`:

    local time = os.date("%H:%M:%S")
    env.TIME = time
    
    return "[$TIME] ($USER@$HOST) $PWD $ "

Alternatively, you can just write out your own prompt:

    io.write( "(", env.USER, "@", env.HOST, ")", env.PWD, " $ " )

## Colours and formatting

The `fmt()` function writes the provided string with the requested formatting:

    env.USER = fmt("red", env.USER)
    env.TIME = fmt("bold", os.date("%H:%M:%S"))

    return "[$TIME] ($USER@$HOST) $PWD $ "

Pass a number as the first argument for xterm colours:

    env.PWD = fmt(208, env.PWD)

## License

See LICENSE.



