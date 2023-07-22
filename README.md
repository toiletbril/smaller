# smaller

A tool that creates `<skin element>.png` from `<skin element>@2x.png` for every `@2x.png` element in an osu! skin.

<[Download Page](https://github.com/toiletbril/smaller/releases/latest)>

## Usage

Open the executable, then select `skin.ini` from skin folder you want to edit. That's it.

### CLI
```console
$ ./smaller [-o] <path to osu! skin>
```

- `-o` to overwrite all files.

**If a skin folder contains weird symbols in it's name, or you have some other error:**

`cd` to it first, and then use `.../smaller .`:
```console
$ cd <path to osu! skin>
$ <path to smaller executable> .
```

## Building

POSIX:

- `gtk+-3.0`
- `clang`

```console
$ ./build.sh
```

Windows (`MinGW/Clang`):
```console
> ./build.bat
```

awesome [stb headers source](https://github.com/nothings/stb) :3c
