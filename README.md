# smaller
**Only Windows for now**

Creates `<skin element>.png` from `<skin element>@2x.png` for every `@2x.png` element. Should work with `@2x.jpg` too.

## Usage:
```console
$ smaller <path to skin>
```

## Building

```console
clang -O2 -Oz -Wall smaller.c -o bin/smaller
```

very cool [stb headers source](https://github.com/nothings/stb) :3