# promptly

promptly is a shell prompt, except I put a Lua in it.

## Installation

Have pkg-config and LuaJIT installed and run `make`, then put `promptly` somewhere on your `$PATH`.

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

The `fmt(string, format)` function writes the provided string with the requested formatting:

    env.USER = fmt(env.USER, "red")
    env.TIME = fmt(os.date("%H:%M:%S"), "bold")

    return "[$TIME] ($USER@$HOST) $PWD $ "

The available formats are:

- Formatting: **reset**, **bright**, **dim**, **underscore**, **blink**, **reverse**, **hidden**
- Foreground colours: **black**, **red**, **green**, **yellow**, **blue**, **magenta**, **cyan**, **white**
- Background colours: **onblack**, **onred**, **ongreen**, **onyellow**, **onblue**, **onmagenta**, **oncyan**, **onwhite**
- xterm foreground colours: pass a number (0-256)

## License

See LICENSE.



