# NE File Reader
> [!WARNING]
> This project is **under development**! Do not expect to have a stable interface.

A lightweight (but powerfull) New Executable (NE) file reader.

## What is a New Executable file?
In order to a program be executed, the operating system (OS) needs to make a bunch of things: load the data and code segments, load the shared libraries, apply memory relocations, ...

As you may be thinking now, loading a program is a quite complex task, and loading a sequence of instructions and data is a little portion of the whole process. All the data and info required by the loader is bundled in your **program's executable**.

There are many formats for executables, here follows a table with the most used nowadays.

| Format name | File extension/s | Operating system
| - | - | -
| PE - Portable Executable | .exe, .dll, .sys, .acm, .ax, .cpl, .drv, .efi, .mui, .ocx, .scr, .tsp, .mun, .msstyles | Microsoft Windows (since NT 3.1)
| ELF - Executable And Linkable Format | none, .axf, .bin, .elf, .o, .out, .prx, .puff, .ko, .mod, and .so | Unix-like systems (linux, solaris, FreeBSD, ...) and some others
| Mach-O | none, .o, .dylib, .kext | Apple macOS, iOS, iPadOS, watchOS, tvOS, and visionOS