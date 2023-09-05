# smaller

A tool that creates `<skin element>.png` from `<skin element>@2x.png` for every
`@2x.png` element in an osu! skin, because you definitely want to edit skins
without desktop environment.

It supports `png` and `jpg` images, output is always `png`.

Prebuilt binaries can be [downloaded here](https://github.com/toiletbril/smaller/releases/latest).

## Usage

Open the executable, then select `skin.ini` from skin folder you want to edit.
That's it.

### CLI

```console
$ ./smaller [-ov] <path to osu! skin>
```

- `-o` to overwrite all files.
- `-v` to display more info about each file.

**If the skin directory name contains weird symbols, or you have other
errors:**

`cd` to it first, and then use `.../smaller .`:
```console
$ cd <path to osu! skin>
$ <path to smaller executable> .
```

## Building

### POSIX

Required only if you wish to have a file dialog:
- `pkg-config`
- `gtk+-3.0`

```console
$ ./build.sh
```

### Windows (`MinGW/Clang`)
```console
> ./build.bat
```

Awesome [stb headers source](https://github.com/nothings/stb) :3c
