# README

Minimalistic PlayStation (PS1) development workflow using the official PsyQ SDK, under Windows XP SP3.

## Structure

Project folder structure for building a basic `triangle`:

```
bin/
├── BUILDCD/
│   ├── BUILDCD.EXE
│   ├── PSXLICENSE.exe
│   └── STRIPISO.exe
├── EMU/
│   └── NO$PSX.EXE
└── PSYQ/
    ├── CCPSX.EXE
    └── CPE2X.EXE
build/
└── triangle/
    ├── MAIN.CPE
    ├── MAIN.EXE
    ├── DISC.IMG
    └── DISC.ISO
src/
└── triangle/
    ├── main.c
    ├── BUILD.bat
    └── RUN.bat
templates/
├── SYSTEM.CNF
├── MAIN.CTI
└── layout.xml
BUILD.bat
```

### Usage

Run `src/triangle/BUILD.bat` to compile the project. This script invokes the main `BUILD.bat` at the root, handles all compilation steps, and writes the final ISO and binaries to `build/triangle/`.

You can also use `src/triangle/RUN.bat`, which builds the project if needed and launches the generated ISO in `NO$PSX` for instant preview.
