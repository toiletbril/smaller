# smaller

Creates `<skin element>.png` from `<skin element>@2x.png` for every `@2x.png` element in an osu! skin. Supports both `jpg` and `png`, outputs `png`.

## Usage
```console
$ smaller <path to osu! skin>
```

**If a skin folder contains weird symbols in it's name, or you have some other error:**

`cd` to it first, and then use `smaller .`:
```console
$ cd <path to osu! skin>
$ smaller .
```

## Building

POSIX:
```console
$ ./build.sh
```

Windows (requires `MinGW/Clang`)
```console
> ./build.bat
```

awesome [stb headers source](https://github.com/nothings/stb) :3c
